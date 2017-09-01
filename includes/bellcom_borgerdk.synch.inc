<?php

use BorgerDk\ArticleService\Client as BorgerDkClient;
use BorgerDk\ArticleService\Resources\Endpoints\GetAllArticles;
use BorgerDk\ArticleService\Resources\Endpoints\GetArticlesByIDs;
use BorgerDk\ArticleService\Resources\Endpoints\GetMunicipalityList;

module_load_include('inc', 'bellcom_borgerdk', 'includes/bellcom_borgerdk.mapper');

/**
 * Fetches the information about the articles (except the content) and creates/updates the database entry.
 * If the article is new or outdated it is marked with a 'resynch' flag.
 */
function bellcom_borgerdk_fetch_articles($lang = 'da') {
  //fetching lang specific articles
  $client = new BorgerDkClient($lang);
  $articles = new GetAllArticles($client);
  $articles = $articles->getResultFormatted();

  //adding lang suffix
  $exisiting_articles_keys = array_keys($articles);
  foreach ($exisiting_articles_keys as &$k) {
    $k .= "-$lang";
  }

  $exisiting_articles = borgerdk_article_load_multiple($exisiting_articles_keys);

  foreach ($articles as $id => $article) {
    $entity_article = NULL;
    if (array_key_exists("$id-$lang", $exisiting_articles)) {
      $entity_article = $exisiting_articles["$id-$lang"];
    }

    //article info is missing in the database
    if (!$entity_article) {
      $entity_article = entity_create('borgerdk_article', array());

      //updating the values. Resynch flag is added automatically
      $entity_article = bellcom_borgerdk_article_map($entity_article, $article);
      $entity_article->entity_id .= "-$lang";
      $entity_article->lang = $lang;

      //only saving properly formatted articles
      if ($entity_article->title) {
        borgerdk_article_save($entity_article);
      }
    }
    elseif (strtotime($article->lastUpdated) > $exisiting_articles["$id-$lang"]->lastUpdated) {
      //updating the values and adding flag for resynch
      $entity_article = bellcom_borgerdk_article_map($entity_article, $article);
      $entity_article->entity_id .= "-$lang";
      $entity_article->resynch = 1;

      borgerdk_article_save($entity_article);
    }
  }
}

/**
 * Loads the articles the need to be resynched.
 * @return array of borgerdk_article entities.
 */
function bellcom_borgedk_get_resynch_articles($lang = 'da') {
  return borgerdk_article_load_multiple(FALSE, array('resynch' => 1, 'lang' => $lang));
}

/**
 * Processes the provided items by updating its information in the database:
 * updates borgerdk_article entity and its child entities: borgerdk_mircoarticle and borgerdk_selfservice.
 *
 * After an update 'resynch' flag is removed each borgerdk_article entity.
 *
 * @param $chunk
 */
function bellcom_borgerdk_queue_worker($chunk) {
  //extracting info from chunk
  $lang = $chunk['lang'];
  $items = $chunk['items'];

  //getting lang specific articles
  $client = new BorgerDkClient($lang);

  //removing lang suffix to perform webservice search
  $chunk_article_ids = array_keys($items);
  foreach ($chunk_article_ids as &$k) {
    $k = str_replace("-$lang",'', $k);
  }
  $params = array('articleIDs' => $chunk_article_ids);
  if (variable_get('bellcom_borgerdk_selected_municipality')) {
    $params['municipalityCode'] = variable_get('bellcom_borgerdk_selected_municipality');
  }

  $articles_resource = new GetArticlesByIDs($client, $params);
  $result_raw = $articles_resource->getResultRaw();

  if (isset($result_raw->Article)) {
    if (is_array($result_raw->Article)) {
      $result_raw = $result_raw->Article;
    }
    else {
      $result_raw = array($result_raw->Article);
    }
  }
  else {
    $result_raw = array($result_raw);
  }

  $existing_articles = borgerdk_article_load_multiple(array_keys($items));

  foreach ($result_raw as $article_raw) {
    $article = NULL;
    try {
      $article = $articles_resource->formatSingleArticle($article_raw);
    } catch (Exception $e) {
      watchdog('Bellcom Borger.dk', 'Ignoring article ID: %id. Error: %error',
        array('%id' => $article_raw->ArticleID, '%error' => $e->getMessage()), WATCHDOG_ERROR);
      continue;
    }

    $entity_article = $existing_articles["$article->id-$lang"];
    if (!$entity_article) {
      watchdog('Bellcom Borger.dk', 'Ignoring article ID: %id. Article does not exist, try fetching it first',
        array('%id' => $article->id), WATCHDOG_WARNING);
      continue;
    }

    //updating the article values
    $entity_article = bellcom_borgerdk_article_map($entity_article, $article);
    $original_article_entity_id = $entity_article->entity_id;
    $entity_article->entity_id .= "-$lang";

    //updating microarticles
    $article_microarticles = array();
    foreach ($article->microArticles as $microarticle) {
      $entity_ma = borgerdk_microarticle_load($microarticle->id . '-' . $original_article_entity_id);
      if (!$entity_ma) {
        $entity_ma = entity_create('borgerdk_microarticle', array());
      }

      $entity_ma = bellcom_borgerdk_microarticle_map($entity_ma, $microarticle);
      $entity_ma->article_id = $entity_article->entity_id;
      $entity_ma->entity_id .= '-' . $original_article_entity_id;
      borgerdk_microarticle_save($entity_ma);

      //saving microarticles to quickly find if any selfservice is related with it
      $article_microarticles[$entity_ma->title] = $entity_ma;
    }

    //updating self-sevice links
    foreach ($article->selfServiceLinks as $selfservice) {
      $entity_ss = borgerdk_selfservice_load($selfservice->id . '-' . $original_article_entity_id);

      if (!$entity_ss) {
        $entity_ss = entity_create('borgerdk_selfservice', array());
      }

      $entity_ss = bellcom_borgerdk_selfservice_map($entity_ss, $selfservice);
      $entity_ss->article_id = $entity_article->entity_id;
      $entity_ss->entity_id .= '-' . $original_article_entity_id;

      //check if we have a microarticle with the similar title
      $related_ma_name = preg_replace('/(Start\s)/i', '', $entity_ss->title);
      if (array_key_exists($related_ma_name, $article_microarticles)) {
        $related_microarticle = $article_microarticles[$related_ma_name];
        $entity_ss->microarticle_id = $related_microarticle->entity_id;
      }
      borgerdk_selfservice_save($entity_ss);
    }

    $entity_article->resynch = 0;
    borgerdk_article_save($entity_article);
  }

  $queue = DrupalQueue::get(BELLCOM_BORGERDK_QUEUE);
  if ($queue->numberOfItems() == 1) {
    //variable_set('borger_articles_cron_last_execute', time());
    watchdog('Bellcom Borger.dk', 'Article processing finished.');
  }
}

/**
 * Returns the list of available municipalities from Borger.dk webservice.
 * Stored in cache for 6 hours.
 *
 * @return array|GetMunicipalityList
 */
function bellcom_borgedk_get_municipalities_list() {
  if ($cache = cache_get('bellcom_borgedk_get_municipalities_list')) {
    $municipalities = $cache->data;
  }
  else {
    $client = new BorgerDkClient();
    $municipalities = new GetMunicipalityList($client);
    $municipalities = $municipalities->getResultFormatted();

    cache_set('bellcom_borgedk_get_municipalities_list', $municipalities, 'cache', strtotime('+6 hour', time()));
  }

  return $municipalities;
}
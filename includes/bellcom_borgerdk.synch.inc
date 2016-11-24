<?php

use BorgerDk\ArticleService\Client as BorgerDkClient;
use BorgerDk\ArticleService\Resources\Endpoints\GetAllArticles;
use BorgerDk\ArticleService\Resources\Endpoints\GetArticlesByIDs;

module_load_include('inc', 'bellcom_borgerdk', 'includes/bellcom_borgerdk.mapper');

function bellcom_borgedk_test(){
  $client = new BorgerDkClient();
  $params = array('articleIDs' => array(35));

  $articles = new GetArticlesByIDs($client, $params);
  $articles->getResultFormatted();

  return 'test';
}
/**
 * Fetches the information about the articles (except the content) and creates/updates the database entry.
 * If the article is new or outdated it is marked with a 'resynch' flag.
 */
function bellcom_borgerdk_fetch_articles() {
  $client = new BorgerDkClient();
  $articles = new GetAllArticles($client);
  $articles = $articles->getResultFormatted();

  $exisiting_articles = borgerdk_article_load_multiple(array_keys($articles));

  foreach ($articles as $id => $article) {
    $entity_article = null;
    if (array_key_exists($id, $exisiting_articles)) {
      $entity_article = $exisiting_articles[$id];
    }

    //article info is missing in the database
    if (!$entity_article) {
      $entity_article = entity_create('borgerdk_article', array());

      //updating the values. Resynch flag is added automatically
      $entity_article = bellcom_borgerdk_article_map($entity_article, $article);

      borgerdk_article_save($entity_article);
    }
    elseif (strtotime($article->lastUpdated) > $exisiting_articles[$id]->lastUpdated) {
      //updating the values and adding flag for resynch
      $entity_article = bellcom_borgerdk_article_map($entity_article, $article);
      $entity_article->resynch = 1;

      borgerdk_article_save($entity_article);
    }
  }
}

/**
 * Loads the articles the need to be resynched.
 * @return array of borgerdk_article entities.
 */
function bellcom_borgedk_get_resynch_articles() {
  return borgerdk_article_load_multiple(false, array('resynch' => 1));
}

/**
 * Processes the provided items by updating its information in the database:
 * updates borgerdk_article entity and its child entities: borgerdk_mircoarticle and borgerdk_selfservice.
 *
 * After an update 'resynch' flag is removed each borgerdk_article entity.
 *
 * @param $items
 */
function bellcom_borgerdk_queue_worker($items) {
  $client = new BorgerDkClient();
  $params = array('articleIDs' => array_keys($items));
  $articles_resource = new GetArticlesByIDs($client, $params);
  $result_raw = $articles_resource->getResultRaw();

  if (isset($result_raw->Article)) {
    if (is_array($result_raw->Article)) {
      $result_raw = $result_raw->Article;
    } else {
      $result_raw = array($result_raw->Article);
    }
  } else {
    $result_raw = array($result_raw);
  }

  $existing_articles = borgerdk_article_load_multiple(array_keys($items));

  foreach($result_raw as $article_raw) {
    $article = null;
    try {
      $article = $articles_resource->formatSingleArticle($article_raw);
    } catch (Exception $e){
      watchdog('Bellcom Borger.dk', 'Ignoring article ID: %id. Error: %error',
        array('%id' => $article_raw->ArticleID, '%error' => $e->getMessage()), WATCHDOG_ERROR);
      continue;
    }

    $entity_article = $existing_articles[$article->id];
    if (!$entity_article) {
      watchdog('Bellcom Borger.dk', 'Ignoring article ID: %id. Article does not exist, try fetching it first',
        array('%id' => $article->id), WATCHDOG_WARNING);
      continue;
    }

    //updating the values. Removing resynch flag
    $entity_article = bellcom_borgerdk_article_map($entity_article, $article);

    //updating microarticles
    $article_microarticles = array();
    foreach ($article->microArticles as $microarticle) {
      $entity_ma = borgerdk_microarticle_load($microarticle->id);
      if (!$entity_ma) {
        $entity_ma = entity_create('borgerdk_microarticle', array());
      }

      $entity_ma = bellcom_borgerdk_microarticle_map($entity_ma, $microarticle);
      $entity_ma->article_id = $entity_article->entity_id;
      borgerdk_microarticle_save($entity_ma);

      //saving microarticles to quickly find if any selfservice is related with it
      $article_microarticles[$entity_ma->title] = $entity_ma;
    }

    //updating self-sevice links
    foreach ($article->selfServiceLinks as $selfservice) {
      $entity_ss = borgerdk_selfservice_load($selfservice->id);

      if (!$entity_ss) {
        $entity_ss = entity_create('borgerdk_selfservice', array());
      }

      $entity_ss = bellcom_borgerdk_selfservice_map($entity_ss, $selfservice);
      $entity_ss->article_id = $entity_article->entity_id;

      //check if we have a microarticle with the same title
      if (array_key_exists($entity_ss->label, $article_microarticles)) {
        $related_microarticle = $article_microarticles[$entity_ss->label];
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
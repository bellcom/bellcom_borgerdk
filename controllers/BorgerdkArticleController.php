<?php

/**
 * BorgerdkArticle class.
 */
class BorgerdkArticleController extends BorgerdkAbstractEntityController {

  /**
   * Sets resynch flag to 1 and delegates to parent create function.
   *
   * @param array $values
   * @return object
   */
  public function create(array $values = array()) {
    $values += array(
      'resynch' => TRUE,
    );
    return parent::create($values);
  }

  /**
   * If empty generates the entity id, fills author information
   * after that delegates to parent save function
   *
   * @param $entity
   * @param DatabaseTransaction $transaction
   * @return bool|int
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (isset($entity->is_new) && $entity->is_new && !isset($entity->entity_id)) {
      $entity->entity_id = $this->generateEntityId($entity);
      $entity->publishingDate = REQUEST_TIME;
      $entity->lastUpdated = REQUEST_TIME;
    }

    if (!isset($entity->uid)) {
      global $user;
      $entity->uid = $user->uid;
    }

    return parent::save($entity, $transaction);
  }

  /**
   * Builds content overview for full and basic info view_mode
   *
   * @param $entity
   * @param string $view_mode
   * @param null $langcode
   * @param array $content
   * @return array
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $weight = 0;

    $default = array(
      '#language' => LANGUAGE_NONE,
      '#label_display' => 'above',
      '#entity_type' => 'borgerdk_article',
      '#bundle' => 'borgerdk_article',
    );

    $content['entity_id'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('Entity ID'),
        '#field_name' => 'entity_id',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->entity_id)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->entity_id))
      ) + $default;

    $content['title'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('Title'),
        '#field_name' => 'title',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->title)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->title))
      ) + $default;

    if ($entity->header) {
    $content['header'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('Header'),
        '#field_name' => 'header',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->header)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->header))
      ) + $default;
    }

    if ($entity->articleUrl) {
    $content['articleUrl'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('Article URL'),
        '#field_name' => 'articleUrl',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->articleUrl)),
        '#formatter' => 'text_default',
        0 => array('#markup' => l($entity->articleUrl, $entity->articleUrl, array('attributes' => array('target' => '_blank'))))
      ) + $default;
    }

    if ($entity->legislation) {
      $content['legislation'] = array(
          '#theme' => 'field',
          '#weight' => $weight++,
          '#title' => t('Legislation'),
          '#field_name' => 'legislation',
          '#field_type' => 'text',
          '#items' => array(array('value' => $entity->legislation)),
          '#formatter' => 'text_default',
          0 => array('#markup' => $entity->legislation)
        ) + $default;
    }

    if ($entity->recommendation) {
      $content['recommendation'] = array(
          '#theme' => 'field',
          '#weight' => $weight++,
          '#title' => t('Recommendation'),
          '#field_name' => 'recommendation',
          '#field_type' => 'text',
          '#items' => array(array('value' => $entity->recommendation)),
          '#formatter' => 'text_default',
          0 => array('#markup' => $entity->recommendation)
        ) + $default;
    }

    if ($content['byline']) {
      $content['byline'] = array(
          '#theme' => 'field',
          '#weight' => $weight++,
          '#title' => t('Byline'),
          '#field_name' => 'byline',
          '#field_type' => 'text',
          '#items' => array(array('value' => $entity->byline)),
          '#formatter' => 'text_default',
          0 => array('#markup' => check_plain($entity->byline))
        ) + $default;
    }

    $microarticle_entities = borgerdk_microarticle_load_multiple_sorted(FALSE, array('article_id' => $entity->entity_id));
    if (!empty($microarticle_entities)) {
      if ($view_mode == 'full') {
        $content['microarticles'] = array(
            '#theme' => 'field',
            '#weight' => $weight++,
            '#title' => t('Microarticles'),
            '#field_name' => 'microarticles',
            '#field_type' => 'entityreference',
            '#formatter' => 'entityreference_entity_view',
          ) + $default;
        foreach ($microarticle_entities as $id => $ma) {
          $content['microarticles']['#items'][$id] = array('target_id' => $id, $ma);
          $content['microarticles'][$id] = entity_view('borgerdk_microarticle', array(entity_id('borgerdk_microarticle', $ma) => $ma), 'teaser');
        }

      }
      else {
        if ($view_mode == 'basic_info') {
          $microarticles_links = array();
          foreach ($microarticle_entities as $ma) {
            $microarticles_links[$ma->entity_id] = array(
              'title' => $ma->title,
              'href' => entity_uri('borgerdk_microarticle', $ma)['path'],
            );
          }

          $content['microarticles'] = array(
              '#theme' => 'links',
              '#weight' => $weight++,
              '#heading' => array('text' => 'Microarticles', 'level' => 'h2'),
              '#field_name' => 'microarticles',
              '#field_type' => 'links',
              '#links' => $microarticles_links,
            ) + $default;
        }
      }
    }

    $selfservice_link_entities = borgerdk_selfservice_load_multiple_sorted(FALSE, array(
      'article_id' => $entity->entity_id,
      'microarticle_id' => NULL
    ));
    if (!(empty($selfservice_link_entities))) {
      if ($view_mode == 'full') {
        $content['selfservices'] = array(
            '#theme' => 'field',
            '#weight' => $weight++,
            '#title' => t('Self-services'),
            '#field_name' => 'selfservices',
            '#field_type' => 'entityreference',
            '#formatter' => 'entityreference_entity_view',
          ) + $default;
        foreach ($selfservice_link_entities as $id => $ss) {
          $content['selfservices']['#items'][$id] = array('target_id' => $id, $ss);
          $content['selfservices'][$id] = entity_view('borgerdk_selfservice', array(entity_id('borgerdk_selfservice', $ss) => $ss), 'teaser');
        }
      }
      else {
        if ($view_mode == 'basic_info') {
          $selfservice_links = array();
          foreach ($selfservice_link_entities as $ss) {
            $selfservice_links[$ss->entity_id] = array(
              'title' => $ss->title,
              'href' => entity_uri('borgerdk_selfservice', $ss)['path'],
            );
          }

          if (!empty($selfservice_links)) {
            $content['selfservices'] = array(
                '#theme' => 'links',
                '#weight' => $weight++,
                '#heading' => array('text' => 'Selfservices', 'level' => 'h2'),
                '#field_name' => 'selfservices',
                '#field_type' => 'links',
                '#links' => $selfservice_links,
              ) + $default;
          }
        }
      }
    }

    $content['publishingDate'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('Publishing Date (Borger.dk)'),
        '#field_name' => 'publishingDate',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->publishingDate)),
        '#formatter' => 'text_default',
        0 => array('#markup' => format_date($entity->publishingDate))
      ) + $default;

    $content['lastUpdated'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('Last Updated (Borger.dk)'),
        '#field_name' => 'lastUpdated',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->lastUpdated)),
        '#formatter' => 'text_default',
        0 => array('#markup' => format_date($entity->lastUpdated))
      ) + $default;

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }

  /**
   * Generates an unique random entity ID
   *
   * @param $entity
   * @return string
   */
  protected function generateEntityId($entity) {
    $hash = crc32(time());
    $entity_id = substr(abs($hash), 0, 5);

    if (!$this->isEntityIdUnique($entity_id)) {
      $entity_id = $this->generateEntityId($entity);
    }
    return $entity_id;
  }

  /**
   * Before deleting the entity itself deletes all its childred - microrticles and self-services,
   * after that delegates to parent delete function.
   *
   * @param $ids
   * @param DatabaseTransaction $transaction
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    foreach ($ids as $id) {
      //deleting self-services
      $selfservices = borgerdk_selfservice_load_multiple(FALSE, array('article_id' => $id), TRUE);
      borgerdk_selfservice_delete_multiple(array_keys($selfservices));

      //deleting microarticles
      $microarticles = borgerdk_microarticle_load_multiple(FALSE, array('article_id' => $id), TRUE);
      borgerdk_microarticle_delete_multiple(array_keys($microarticles));
    }

    parent::delete($ids, $transaction);
  }
}
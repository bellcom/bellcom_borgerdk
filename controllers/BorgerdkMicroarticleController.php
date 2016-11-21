<?php

/**
 * BorgerdkMicroarticle class.
 */
class BorgerdkMicroarticleController extends EntityAPIController {

  public function create(array $values = array()) {
    $values += array(
      'title' => '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
    );
    return parent::create($values);
  }

  public function save($entity) {
    $entity->changed = REQUEST_TIME;
    return parent::save($entity);
  }

  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $weight = 0;

    $default = array(
      '#language' => LANGUAGE_NONE,
      '#label_display' => 'above',
      '#entity_type' => 'borgerdk_microarticle',
      '#bundle' => 'borgerdk_microarticle',
    );

    $content['entity_id'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' =>t('Entity ID'),
        '#field_name' => 'entity_id',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->entity_id)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->entity_id))
      ) + $default;

    $content['title'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' =>t('Title'),
        '#field_name' => 'title',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->title)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->title))
      ) + $default;

    $content['content'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' =>t('Content'),
        '#field_name' => 'content',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->content)),
        '#formatter' => 'text_default',
        0 => array('#markup' => strip_tags($entity->content))
      ) + $default;

    $selfservice_link_entities = borgerdk_selfservice_load_multiple(false, array('microarticle_id' => $entity->entity_id));
    if (!(empty($selfservice_link_entities))) {
      if ($view_mode == 'teaser') {
        $content['selfservices'] = array(
            '#theme' => 'field',
            '#weight' => $weight++,
            '#title' => t('Self-services'),
            '#field_name' => 'selfservices',
            '#field_type' => 'entityreference',
            '#formatter' => 'entityreference_entity_view',
          ) + $default;
        foreach($selfservice_link_entities as $id => $ss) {
          $content['selfservices']['#items'][$id] = array('target_id' => $id, $ss);
          $content['selfservices'][$id] = entity_view('borgerdk_selfservice', array(entity_id('borgerdk_selfservice', $ss) => $ss), 'teaser');
        }
      } else if ($view_mode == 'full'){
        $selfservice_links = array();
        foreach($selfservice_link_entities as $ss) {
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

    if ($view_mode == 'full') {
      $parent_article = borgerdk_article_load($entity->article_id);
      if ($parent_article) {
        $content['article_id'] = array(
            '#theme' => 'field',
            '#weight' => $weight++,
            '#title' =>t('Parent Article'),
            '#field_name' => 'article_id',
            '#field_type' => 'text',
            '#items' => array(array('value' => $parent_article->title)),
            '#formatter' => 'text_default',
            0 => array('#markup' =>  l($parent_article->title, entity_uri('borgerdk_article', $parent_article)['path']))
          ) + $default;
      }
    }

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }
} 
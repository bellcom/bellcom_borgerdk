<?php

/**
 * BorgerdkSelfservice class.
 */
class BorgerdkSelfserviceController extends EntityAPIController {

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
      '#entity_type' => 'borgerdk_selfservice',
      '#bundle' => 'borgerdk_selfservice',
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

    $content['label'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' =>t('label'),
        '#field_name' => 'label',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->label)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->label))
      ) + $default;

    $content['url'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' =>t('URL'),
        '#field_name' => 'url',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->url)),
        '#formatter' => 'text_default',
        0 => array('#markup' =>  l($entity->url, $entity->url, array('attributes' => array('target'=>'_blank'))))
      ) + $default;

    if ($view_mode == 'full') {
      if ($entity->microarticle_id) {
        $parent_microarticle = borgerdk_microarticle_load($entity->microarticle_id);
        if ($parent_microarticle) {
          $content['microarticle_id'] = array(
              '#theme' => 'field',
              '#weight' => $weight++,
              '#title' =>t('Parent Microarticle'),
              '#field_name' => 'microarticle_id',
              '#field_type' => 'text',
              '#items' => array(array('value' => $parent_microarticle->title)),
              '#formatter' => 'text_default',
              0 => array('#markup' =>  l($parent_microarticle->title, entity_uri('borgerdk_microarticle', $parent_microarticle)['path']))
            ) + $default;
        }
      } else {
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
    }

    return parent::buildContent($entity, $view_mode, $langcode, $content);

  }
}
<?php

/**
 * BorgerdkMicroarticle class.
 */
class BorgerdkMicroarticleController extends BorgerdkAbstractEntityController {

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
      $entity->entity_id = parent::generateEntityId($entity);
    }
    global $user;
    $entity->uid = $user->uid;
    //default setting = creating a new revision if not mentioned otherwise
    if (!isset($entity->is_new_revision)) {
      $entity->is_new_revision = TRUE;
    }

    // Calculates the weight of the new microarticle as the weight of the max + 1;
    if (!isset($entity->weight)) {
      $existing_mm = borgerdk_microarticle_load_multiple(FALSE, array('article_id' => $entity->article_id));
      $max_weight = 0;
      foreach ($existing_mm as $mm) {
        if ($mm->weight > $max_weight) {
          $max_weight = $mm->weight;
        }
      }
      $max_weight++;
      $entity->weight = $max_weight;
    }
    return parent::save($entity, $transaction);
  }

  /**
   * Builds entity overview for full and teaser view_modes.
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
      '#entity_type' => 'borgerdk_microarticle',
      '#bundle' => 'borgerdk_microarticle',
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

    $content['content'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('Content'),
        '#field_name' => 'content',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->content)),
        '#formatter' => 'text_default',
        0 => array('#markup' => $entity->content)
      ) + $default;

    $selfservice_link_entities = borgerdk_selfservice_load_multiple_sorted(FALSE, array('microarticle_id' => $entity->entity_id));
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
        foreach ($selfservice_link_entities as $id => $ss) {
          $content['selfservices']['#items'][$id] = array('target_id' => $id, $ss);
          $content['selfservices'][$id] = entity_view('borgerdk_selfservice', array(entity_id('borgerdk_selfservice', $ss) => $ss), 'teaser');
        }
      }
      else {
        if ($view_mode == 'full') {
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

    if ($view_mode == 'full') {
      $parent_article = borgerdk_article_load($entity->article_id);
      if ($parent_article) {
        $content['article_id'] = array(
            '#theme' => 'field',
            '#weight' => $weight++,
            '#title' => t('Parent Article'),
            '#field_name' => 'article_id',
            '#field_type' => 'text',
            '#items' => array(array('value' => $parent_article->title)),
            '#formatter' => 'text_default',
            0 => array('#markup' => l($parent_article->title, entity_uri('borgerdk_article', $parent_article)['path']))
          ) + $default;
      }

      if ($entity->uid) {
        $author = user_load($entity->uid);
        $content['author'] = array(
            '#theme' => 'field',
            '#weight' => $weight++,
            '#title' => t('Author'),
            '#field_name' => 'author',
            '#field_type' => 'text',
            '#items' => array(array('value' => $author->name)),
            '#formatter' => 'text_default',
            0 => array('#markup' => l($author->name, entity_uri('user', $author)['path']))
          ) + $default;
      }
    }

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }

  /**
   * Before deleting the entity itself the content makes all child self-services orphaned.
   *
   * @param $ids
   * @param DatabaseTransaction $transaction
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    //orphaning self-services
    foreach ($ids as $id) {
      $selfservices = borgerdk_selfservice_load_multiple(FALSE, array('microarticle_id' => $id), TRUE);
      foreach ($selfservices as $ss) {
        $ss->microarticle_id = NULL;
        borgerdk_selfservice_save($ss);
      }
    }

    parent::delete($ids, $transaction);
  }
}
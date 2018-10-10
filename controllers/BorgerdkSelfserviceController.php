<?php

/**
 * BorgerdkSelfservice class.
 */
class BorgerdkSelfserviceController extends BorgerdkAbstractEntityController {

  /**
   * If empty generates the entity id, fills author information
   * after that delegates to parent save function
   *
   * @param $entity
   * @param DatabaseTransaction $transaction
   * @return bool|int
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (!isset($entity->entity_id)) {
      $entity->entity_id = parent::generateEntityId($entity);
    }

    global $user;
    if (!isset($entity->uid)){
      $entity->uid = $user->uid;
    }

    if (isset($entity->is_new) && $entity->is_new && !$entity->microarticle_id) {
      //if we are adding new microarticle, automatically enable it for all nodes, referencing that article
      $this->addSelfserviceReferences($entity);
    }

    //default setting = creating a new revisiton is not mentioned otherwise
    if (!isset($entity->is_new_revision)) {
      $entity->is_new_revision = TRUE;
    }

    //if microarticle_id is 0 set it to null, to keep the data consistent
    if (isset($entity->microarticle_id) && $entity->microarticle_id === 0) {
      $entity->microarticle_id = NULL;
    }

    // Calculates the weight of the new self-service as the weight of the max + 1;
    if (!isset($entity->weight)) {
      $existing_ss = borgerdk_selfservice_load_multiple(FALSE, array('article_id' => $entity->article_id));
      $max_weight = 0;
      foreach ($existing_ss as $ss) {
        if ($ss->weight > $max_weight) {
          $max_weight = $ss->weight;
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
      '#entity_type' => 'borgerdk_selfservice',
      '#bundle' => 'borgerdk_selfservice',
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

    $content['label'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('Label'),
        '#field_name' => 'label',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->label)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->label))
      ) + $default;

    $content['url'] = array(
        '#theme' => 'field',
        '#weight' => $weight++,
        '#title' => t('URL'),
        '#field_name' => 'url',
        '#field_type' => 'text',
        '#items' => array(array('value' => $entity->url)),
        '#formatter' => 'text_default',
        0 => array('#markup' => l($entity->url, $entity->url, array('attributes' => array('target' => '_blank'))))
      ) + $default;

    if ($view_mode == 'full') {
      if ($entity->microarticle_id) {
        $parent_microarticle = borgerdk_microarticle_load($entity->microarticle_id);
        if ($parent_microarticle) {
          $content['microarticle_id'] = array(
              '#theme' => 'field',
              '#weight' => $weight++,
              '#title' => t('Parent Microarticle'),
              '#field_name' => 'microarticle_id',
              '#field_type' => 'text',
              '#items' => array(array('value' => $parent_microarticle->title)),
              '#formatter' => 'text_default',
              0 => array('#markup' => l($parent_microarticle->title, entity_uri('borgerdk_microarticle', $parent_microarticle)['path']))
            ) + $default;
        }
      }
      else {
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
   * This function handles calls to removeSelfserviceReferences.
   * Then it calls to the parent class to do the actual entity deletion.
   *
   * @param $ids
   * @param DatabaseTransaction $transaction
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    // Removing selfservice references.
    $selfservices_to_delete = borgerdk_selfservice_load_multiple($ids);
    foreach ($selfservices_to_delete as $ss_to_delete) {
      $this->removeSelfserviceReferences($ss_to_delete);
    }

    // Finally letting selfservice to be deleted.
    parent::delete($ids, $transaction);
  }

  /**
   * This function adds the selfservice references to the affected nodes.
   * Affected node is any nodes referencing the article of selfservice.
   *
   * @param $selfservice
   */
  protected function addSelfserviceReferences($selfservice) {
    // Getting all fields of type borgerdk_article_field.
    $fields = field_read_fields(array('type' => 'borgerdk_article_field'));
    foreach ($fields as $field) {
      $field_instances = field_read_instances(array('field_id' => $field['id']));

      // Looping through fields to get affected nodes.
      foreach ($field_instances as $field_instance) {
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', $field_instance['entity_type'])
          ->entityCondition('bundle', $field_instance['bundle'])
          ->fieldCondition($field_instance['field_name'], 'borgerdk_article_entity_id', $selfservice->article_id);

        $result = $query->execute();
        if (isset($result['node'])) {
          $nids = array_keys($result['node']);
          $affected_nodes = entity_load('node', $nids);

          foreach ($affected_nodes as $affected_node) {
            foreach ($affected_node->{$field_instance['field_name']}['und'] as $delta => $node_field) {
              $enabled_selfservices = $affected_node->{$field_instance['field_name']}['und'][$delta]['borgerdk_selfservice_entity_ids'];
              $enabled_selfservices = json_decode($enabled_selfservices);

              // Adding this selfservice
              $enabled_selfservices[] = $selfservice->entity_id;

              $enabled_selfservices = array_values($enabled_selfservices);
              $affected_node->{$field_instance['field_name']}['und'][$delta]['borgerdk_selfservice_entity_ids'] = json_encode($enabled_selfservices);
            }
            node_save($affected_node);
          }
        }
      }
    }
  }

  /**
   * This function removes the selfservice references from the affected nodes.
   * Affected node is any nodes referencing the article of selfservice.
   *
   * @param $selfservice
   */
  protected function removeSelfserviceReferences($selfservice) {
    // Getting all fields of type borgerdk_article_field.
    $fields = field_read_fields(array('type' => 'borgerdk_article_field'));
    foreach ($fields as $field) {
      $field_instances = field_read_instances(array('field_id' => $field['id']));

      // Looping through fields to get affected nodes.
      foreach ($field_instances as $field_instance) {
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', $field_instance['entity_type'])
          ->entityCondition('bundle', $field_instance['bundle'])
          ->fieldCondition($field_instance['field_name'], 'borgerdk_article_entity_id', $selfservice->article_id);

        $result = $query->execute();
        if (isset($result['node'])) {
          $nids = array_keys($result['node']);
          $affected_nodes = entity_load('node', $nids);

          foreach ($affected_nodes as $affected_node) {
            foreach ($affected_node->{$field_instance['field_name']}['und'] as $delta => $node_field) {
              $enabled_selfservices = $affected_node->{$field_instance['field_name']}['und'][$delta]['borgerdk_selfservice_entity_ids'];
              $enabled_selfservices = json_decode($enabled_selfservices);
              // Getting key to unset.
              $unset_ss_key = array_search($selfservice->entity_id, $enabled_selfservices);
              unset($enabled_selfservices[$unset_ss_key]);

              $enabled_selfservices = array_values($enabled_selfservices);
              $affected_node->{$field_instance['field_name']}['und'][$delta]['borgerdk_selfservice_entity_ids'] = json_encode($enabled_selfservices);
            }
            node_save($affected_node);
          }
        }
      }
    }
  }
}
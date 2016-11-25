<?php

/**
 * Helper function for creating microarticle related menu items
 *
 * @return array
 */
function bellcom_borgerdk_microarticle_menu() {
  $items = array();

  $microarticle_uri = 'borgerdk/microarticle/%borgerdk_microarticle';
  $microarticle_uri_argument_position = 2;
  $items[$microarticle_uri] = array(
    'title callback' => 'entity_label',
    'title arguments' => array('borgerdk_microarticle', $microarticle_uri_argument_position),
    'page callback' => 'bellcom_borgerdk_microarticle_view',
    'page arguments' => array($microarticle_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('view', 'borgerdk_microarticle', $microarticle_uri_argument_position),
    'file' => 'includes/bellcom_borgerdk.pages.inc',
  );
  $items[$microarticle_uri . '/view'] = array(
    'title' => 'View',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -10,
  );
  $items[$microarticle_uri . '/edit'] = array(
    'title' => 'Edit',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('borgerdk_microarticle_form', $microarticle_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('update', 'borgerdk_microarticle', $microarticle_uri_argument_position),
    'type' => MENU_LOCAL_TASK,
    'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    'weight' => 0,
  );
  $items[$microarticle_uri . '/delete'] = array(
    'title' => 'Delete microarticle',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('bellcom_borgerdk_microarticle_delete_form', $microarticle_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('delete', 'borgerdk_microarticle', $microarticle_uri_argument_position),
    'weight' => 2,
    'type' => MENU_LOCAL_TASK,
    'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
  );
  $items[$microarticle_uri . '/revisions'] = array(
    'title' => 'Revisions',
    'page callback' => 'bellcom_borgerdk_microarticle_revision_overview',
    'page arguments' => array($microarticle_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('update', 'borgerdk_microarticle', $microarticle_uri_argument_position),
    'weight' => 1,
    'type' => MENU_LOCAL_TASK,
  );
  $items[$microarticle_uri . '/revisions/%/view'] = array(
    'title' => 'Revisions',
    'load arguments' => array(4),
    'page callback' => 'bellcom_borgerdk_microarticle_view',
    'page arguments' => array($microarticle_uri_argument_position, TRUE),
    'access callback' => 'entity_access',
    'access arguments' => array('view', 'borgerdk_microarticle', $microarticle_uri_argument_position),
    'file' => 'includes/bellcom_borgerdk.pages.inc',
  );
  $items[$microarticle_uri . '/revisions/%/revert'] = array(
    'title' => 'Revert to earlier revision',
    'load arguments' => array(4),
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'bellcom_borgerdk_microarticle_revision_revert_confirm',
      $microarticle_uri_argument_position
    ),
    'access callback' => 'entity_access',
    'access arguments' => array('update', 'borgerdk_microarticle', $microarticle_uri_argument_position),
  );
  $items[$microarticle_uri . '/revisions/%/delete'] = array(
    'title' => 'Delete earlier revision',
    'load arguments' => array(4),
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'bellcom_borgerdk_microarticle_revision_delete_confirm',
      $microarticle_uri_argument_position
    ),
    'access callback' => 'entity_access',
    'access arguments' => array('delete', 'borgerdk_microarticle', $microarticle_uri_argument_position),
  );

  return $items;
}

/**
 * Helper function for generating microarticle entity info
 *
 * @return array
 */
function bellcom_borgedk_microarticle_entity_info() {
  return array(
    'borgerdk_microarticle' => array(
      'label' => t('Borger.dk Microarticle'),
      'entity class' => 'BorgerdkMicroarticleEntity',
      'controller class' => 'BorgerdkMicroarticleController',
      'base table' => 'borgerdk_microarticle',
      'revision table' => 'borgerdk_microarticle_revision',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'entity_id',
        'revision' => 'vid',
      ),
      'load hook' => 'borgerdk_microarticle_load',
      'view modes' => array(
        'full' => array(
          'label' => t('Default'),
          'custom settings' => FALSE,
        ),
      ),
      'label callback' => 'entity_class_label',
      'uri callback' => 'entity_class_uri',
      'access callback' => 'borgerdk_microarticle_access',
      'admin ui' => array(
        'path' => 'admin/content/borgerdk-microarticles',
        'controller class' => 'BorgerdkMicroarticleUIController',
      ),
    ),
  );
}

function borgerdk_microarticle_load($entity_id = NULL, $vid = NULL, $reset = FALSE) {
  $conditions = (isset($vid) ? array('vid' => $vid) : array());
  $entities = borgerdk_microarticle_load_multiple(array($entity_id), $conditions, $reset);
  return reset($entities);
}

function borgerdk_microarticle_load_multiple($entity_ids = FALSE, $conditions = array(), $reset = FALSE) {
  return entity_load('borgerdk_microarticle', $entity_ids, $conditions, $reset);
}

function borgerdk_microarticle_save($entity) {
  entity_save('borgerdk_microarticle', $entity);
}

function borgerdk_microarticle_delete($entity) {
  entity_delete('borgerdk_microarticle', entity_id('borgerdk_microarticle', $entity));
}

function borgerdk_microarticle_delete_multiple($entity_ids) {
  entity_delete_multiple('borgerdk_microarticle', $entity_ids);
}

function borgerdk_microarticle_delete_revision($revision_id) {
  entity_revision_delete('borgerdk_microarticle', $revision_id);
}

function borgerdk_microarticle_access($op, $entity, $account = NULL, $entity_type) {
  if ($op == 'create') {
    return user_access('administer borgerdk_article');
  }
  if ($entity) {
    //relay on article access
    $article = borgerdk_article_load($entity->article_id);
    return borgerdk_article_access($op, $article, $account, 'borgerdk_article');
  }
}
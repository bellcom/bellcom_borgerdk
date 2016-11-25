<?php

/**
 * Helper function for creating self-service related menu items
 *
 * @return array
 */
function bellcom_borgerdk_selfservice_menu() {
  $items = array();

  $selfservice_uri = 'borgerdk/selfservice/%borgerdk_selfservice';
  $selfservice_uri_argument_position = 2;
  $items[$selfservice_uri] = array(
    'title callback' => 'entity_label',
    'title arguments' => array('borgerdk_selfservice', $selfservice_uri_argument_position),
    'page callback' => 'bellcom_borgerdk_selfservice_view',
    'page arguments' => array($selfservice_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('view', 'borgerdk_selfservice', $selfservice_uri_argument_position),
    'file' => 'includes/bellcom_borgerdk.pages.inc',
  );
  $items[$selfservice_uri . '/view'] = array(
    'title' => 'View',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -10,
  );
  $items[$selfservice_uri . '/edit'] = array(
    'title' => 'Edit',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('borgerdk_selfservice_form', $selfservice_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('update', 'borgerdk_selfservice', $selfservice_uri_argument_position),
    'type' => MENU_LOCAL_TASK,
    'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    'weight' => 0,
  );
  $items[$selfservice_uri . '/delete'] = array(
    'title' => 'Delete self-service',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('bellcom_borgerdk_selfservice_delete_form', $selfservice_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('delete', 'borgerdk_selfservice', $selfservice_uri_argument_position),
    'type' => MENU_LOCAL_TASK,
    'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    'weight' => 2,
  );
  $items[$selfservice_uri . '/revisions'] = array(
    'title' => 'Revisions',
    'page callback' => 'bellcom_borgerdk_selfservice_revision_overview',
    'page arguments' => array($selfservice_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('update', 'borgerdk_selfservice', $selfservice_uri_argument_position),
    'weight' => 1,
    'type' => MENU_LOCAL_TASK,
  );
  $items[$selfservice_uri . '/revisions/%/view'] = array(
    'title' => 'Revisions',
    'load arguments' => array(4),
    'page callback' => 'bellcom_borgerdk_selfservice_view',
    'page arguments' => array($selfservice_uri_argument_position, TRUE),
    'access callback' => 'entity_access',
    'access arguments' => array('view', 'borgerdk_selfservice', $selfservice_uri_argument_position),
    'file' => 'includes/bellcom_borgerdk.pages.inc',
  );
  $items[$selfservice_uri . '/revisions/%/revert'] = array(
    'title' => 'Revert to earlier revision',
    'load arguments' => array(4),
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'bellcom_borgerdk_selfservice_revision_revert_confirm',
      $selfservice_uri_argument_position
    ),
    'access callback' => 'entity_access',
    'access arguments' => array('update', 'borgerdk_selfservice', $selfservice_uri_argument_position),
  );
  $items[$selfservice_uri . '/revisions/%/delete'] = array(
    'title' => 'Delete earlier revision',
    'load arguments' => array(4),
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'bellcom_borgerdk_selfservice_revision_delete_confirm',
      $selfservice_uri_argument_position
    ),
    'access callback' => 'entity_access',
    'access arguments' => array('delete', 'borgerdk_selfservice', $selfservice_uri_argument_position),
  );

  return $items;
}

/**
 * Helper function for generating self-service entity info
 *
 * @return array
 */
function bellcom_borgerdk_selfservice_entity_info() {
  return array(
    'borgerdk_selfservice' => array(
      'label' => t('Borger.dk Selfservice'),
      'entity class' => 'BorgerdkSelfserviceEntity',
      'controller class' => 'BorgerdkSelfserviceController',
      'base table' => 'borgerdk_selfservice',
      'revision table' => 'borgerdk_selfservice_revision',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'entity_id',
        'revision' => 'vid',
      ),
      'load hook' => 'borgerdk_selfservice_load',
      'view modes' => array(
        'full' => array(
          'label' => t('Default'),
          'custom settings' => FALSE,
        ),
      ),
      'label callback' => 'entity_class_label',
      'uri callback' => 'entity_class_uri',
      'access callback' => 'borgerdk_selfservice_access',
      'admin ui' => array(
        'path' => 'admin/content/borgerdk-selfservices',
        'controller class' => 'BorgerdkSelfserviceUIController',
      ),
    )
  );
}

function borgerdk_selfservice_load($entity_id = NULL, $vid = NULL, $reset = FALSE) {
  $conditions = (isset($vid) ? array('vid' => $vid) : array());
  $entities = borgerdk_selfservice_load_multiple(array($entity_id), $conditions, $reset);
  return reset($entities);
}

function borgerdk_selfservice_load_multiple($entity_ids = FALSE, $conditions = array(), $reset = FALSE) {
  return entity_load('borgerdk_selfservice', $entity_ids, $conditions, $reset);
}

function borgerdk_selfservice_save($entity) {
  entity_save('borgerdk_selfservice', $entity);
}

function borgerdk_selfservice_delete($entity) {
  entity_delete('borgerdk_selfservice', entity_id('borgerdk_selfservice', $entity));
}

function borgerdk_selfservice_delete_multiple($entity_ids) {
  entity_delete_multiple('borgerdk_selfservice', $entity_ids);
}

function borgerdk_selfservice_delete_revision($revision_id) {
  entity_revision_delete('borgerdk_selfservice', $revision_id);
}

function borgerdk_selfservice_access($op, $entity, $account = NULL, $entity_type) {
  if ($op == 'create') {
    return user_access('administer borgerdk_article');
  }
  if ($entity) {
    //relay on article access
    $article = borgerdk_article_load($entity->article_id);
    return borgerdk_article_access($op, $article, $account, 'borgerdk_article');
  }
}
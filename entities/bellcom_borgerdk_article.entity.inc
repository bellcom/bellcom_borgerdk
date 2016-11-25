<?php

/**
 * Helper function for creating article related menu items
 *
 * @return array
 */
function bellcom_borgedk_article_menu() {
  $items = array();

  $article_uri = 'borgerdk/article/%borgerdk_article';
  $article_uri_argument_position = 2;
  $items[$article_uri] = array(
    'title callback' => 'entity_label',
    'title arguments' => array('borgerdk_article', $article_uri_argument_position),
    'page callback' => 'bellcom_borgerdk_article_view',
    'page arguments' => array($article_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('view', 'borgerdk_article', $article_uri_argument_position),
    'file' => 'includes/bellcom_borgerdk.pages.inc',
  );
  $items[$article_uri . '/view'] = array(
    'title' => 'View',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -10,
  );
  $items[$article_uri . '/delete'] = array(
    'title' => 'Delete Borger.dk Article',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('bellcom_borgerdk_article_delete_form', $article_uri_argument_position),
    'access callback' => 'entity_access',
    'access arguments' => array('delete', 'borgerdk_article', $article_uri_argument_position),
    'type' => MENU_LOCAL_TASK,
    'weight' => 0,
  );

  return $items;
}

/**
 * Helper function for generating article info
 *
 * @return array
 */
function bellcom_borgedk_article_entity_info() {
  return array(
    'borgerdk_article' => array(
      'label' => t('Borger.dk Article'),
      'entity class' => 'BorgerdkArticleEntity',
      'controller class' => 'BorgerdkArticleController',
      'base table' => 'borgerdk_article',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'entity_id',
      ),
      'load hook' => 'borgerdk_article_load',
      'view modes' => array(
        'full' => array(
          'label' => t('Default'),
          'custom settings' => TRUE,
        ),
      ),
      'label callback' => 'entity_class_label',
      'uri callback' => 'entity_class_uri',
      'access callback' => 'borgerdk_article_access',
      'admin ui' => array(
        'path' => 'admin/content/borgerdk-articles',
        'controller class' => 'BorgerdkArticleUIController',
      ),
    )
  );
}

function borgerdk_article_load($entity_id = NULL, $reset = FALSE) {
  $articles = borgerdk_article_load_multiple(array($entity_id), array(), $reset);
  return reset($articles);
}

function borgerdk_article_load_multiple($entity_ids = FALSE, $conditions = array(), $reset = FALSE) {
  return entity_load('borgerdk_article', $entity_ids, $conditions, $reset);
}

function borgerdk_article_save($entity) {
  entity_save('borgerdk_article', $entity);
}

function borgerdk_article_delete($entity) {
  entity_delete('borgerdk_article', entity_id('borgerdk_article', $entity));
}

function borgerdk_article_delete_multiple($entity_ids) {
  entity_delete_multiple('borgerdk_article', $entity_ids);
}

function borgerdk_article_access($op, $entity, $account = NULL, $entity_type) {
  if ($op == 'create') {
    return user_access('administer borgerdk_article');
  }
  if (isset($entity)) {
    $account = isset($account) ? $account : $GLOBALS['user'];
    if (user_access("$op borgerdk_article", $account)) {
      return TRUE;
    }
  }
}
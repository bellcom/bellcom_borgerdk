<?php

function borgerdk_article_form($form, &$form_state, $article = NULL) {
  //TODO: implement form
  return $form;
}

/**
 * Article delete confirmation form.
 */
function bellcom_borgerdk_article_delete_form($form, &$form_state, $article) {
  $form_state['article'] = $article;

  // Always provide entity id in the same form key as in the entity edit form.
  $form['entity_id'] = array('#type' => 'value', '#value' => entity_id('borgerdk_article', $article));

  $entity_uri = entity_uri('borgerdk_article', $article);
  return confirm_form($form,
    t('Are you sure you want to delete Borger.dk article %article?', array('%article' => entity_label('borgerdk_article', $article))),
    $entity_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Article delete form submit handler.
 */
function bellcom_borgerdk_article_delete_form_submit($form, &$form_state) {
  $article = $form_state['article'];
  borgerdk_article_delete($article);

  drupal_set_message(t('Borger.dk article %title deleted.', array('%title' => entity_label('borgerdk_article', $article))));

  $form_state['redirect'] = '<front>';
}

/**
 * Microarticle edit/create form
 *
 * @param $form
 * @param $form_state
 * @param $microarticle
 * @param string $op
 */
function borgerdk_microarticle_form($form, &$form_state, $microarticle = NULL) {
  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#size' => 50,
  );

  $form['content'] = array(
    '#title' => t('Content'),
    '#type' => 'text_format',
    '#format' => 'full_html',
  );

  $article_options = bellcom_borgerdk_field_get_articles_options();
  $form['article_id'] = array(
    '#title' => t('Borger.dk article'),
    '#type' => 'select',
    '#options' => $article_options,
    '#required' => TRUE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 40,
  );

  //loading default values or creating new entity
  if ($microarticle && !empty($microarticle->entity_id)) {
    $form['title']['#default_value'] = $microarticle->title;
    $form['content']['#default_value'] = $microarticle->content;
    $form['article_id']['#default_value'] = $microarticle->article_id;
  }
  else {
    $microarticle = entity_create('borgerdk_microarticle', array());
  }
  $form_state['microarticle'] = $microarticle;

  return $form;
}

/**
 * Submit handler for creating/editing microarticle.
 */
function borgerdk_microarticle_form_submit(&$form, &$form_state) {
  $microarticle = $form_state['microarticle'];
  entity_form_submit_build_entity('borgerdk_microarticle', $microarticle, $form, $form_state);

  $microarticle->content = $microarticle->content['value'];

  borgerdk_microarticle_save($microarticle);
  $uri = entity_uri('borgerdk_microarticle', $microarticle);

  $form_state['redirect'] = $uri['path'];
  drupal_set_message(t('Microarticle %title saved.', array('%title' => entity_label('borgerdk_microarticle', $microarticle))));
}

/**
 * Microarticle delete confirmation form.
 */
function bellcom_borgerdk_microarticle_delete_form($form, &$form_state, $microarticle) {
  $form_state['microarticle'] = $microarticle;

  // Always provide entity id in the same form key as in the entity edit form.
  $form['entity_id'] = array('#type' => 'value', '#value' => entity_id('borgerdk_microarticle', $microarticle));

  $entity_uri = entity_uri('borgerdk_microarticle', $microarticle);
  return confirm_form($form,
    t('Are you sure you want to delete Borger.dk microarticle %microarticle?', array('%microarticle' => entity_label('borgerdk_microarticle', $microarticle))),
    $entity_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Microarticle delete form submit handler.
 */
function bellcom_borgerdk_microarticle_delete_form_submit($form, &$form_state) {
  $microarticle = $form_state['microarticle'];
  borgerdk_microarticle_delete($microarticle);

  drupal_set_message(t('Borger.dk microarticle %title deleted.', array('%title' => entity_label('borgerdk_microarticle', $microarticle))));

  $form_state['redirect'] = '<front>';
}

function borgerdk_selfservice_form($form, &$form_state, $selfservice = NULL) {
  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#size' => 50,
  );

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#size' => 50,
  );

  $form['url'] = array(
    '#title' => t('URL'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#size' => 100,
  );

  $article_options = bellcom_borgerdk_field_get_articles_options();
  $form['article_id'] = array(
    '#title' => t('Borger.dk article'),
    '#type' => 'select',
    '#options' => $article_options,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'bellcom_borgerdk_admin_reload_microarticles',
      'wrapper' => 'borgerdk-microarticle-wrapper',
    ),
  );

  $form['microarticle_id'] = array(
    '#prefix' => '<div id="borgerdk-microarticle-wrapper">',
    '#title' => t('Borger.dk microarticle'),
    '#type' => 'select',
    '#options' => _bellcom_borgedk_admin_get_microarticle_options($form_state),
    '#required' => FALSE,
    '#empty_value' => 0,
    '#description' => t('Select only if you want self-service to be displayed as part of the microarticle'),
    '#suffix' => '</div>'
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 40,
  );

  //loading default values or creating new entity
  if ($selfservice && !empty($selfservice->entity_id)) {
    $form['title']['#default_value'] = $selfservice->title;
    $form['label']['#default_value'] = $selfservice->label;
    $form['url']['#default_value'] = $selfservice->url;
    $form['article_id']['#default_value'] = $selfservice->article_id;
    if (isset($selfservice->article_id)) {
      $form['microarticle_id']['#options'] =
        _bellcom_borgedk_admin_get_microarticle_options($form_state, $selfservice->article_id);
    }
    if (isset($selfservice->microarticle_id)) {
      $form['microarticle_id']['#default_value'] = $selfservice->microarticle_id;
    }
  }
  else {
    $selfservice = entity_create('borgerdk_selfservice', array());
  }
  $form_state['selfservice'] = $selfservice;

  return $form;
}

/**
 * Ajax callback to reload list of microarticles
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function bellcom_borgerdk_admin_reload_microarticles($form, &$form_state) {
  return $form['microarticle_id'];
}

/**
 * Function to facilitate microarticle select options.
 * In order to allow the ajax reloading of the options, function takes default $article_entity_id to load the options on page load
 * and form_state to dynamically extract triggering element from it.
 * Triggering element is Borger.dk article select dropdown.
 *
 * @param $form_state - current state of the form
 * @param null $article_entity_id - default value, used if form_state['triggering_element'] is empty
 * @return array - formatted array of select options
 */
function _bellcom_borgedk_admin_get_microarticle_options($form_state, $article_entity_id = FALSE) {
  $options = array();

  //checking that trigger element is borgerdk_article_entity_id
  if (isset($form_state['triggering_element']) && $form_state['triggering_element']['#name'] == 'article_id') {
    $article_entity_id = $form_state['triggering_element']['#value'];
  }

  if ($article_entity_id) {
    //load microarticles
    $entities = borgerdk_microarticle_load_multiple(FALSE, array('article_id' => $article_entity_id), TRUE);
    foreach ($entities as $entity) {
      $options[$entity->entity_id] = $entity->title;
    }
  }

  return $options;
}

/**
 * Submit handler for creating/editing microarticle.
 */
function borgerdk_selfservice_form_submit(&$form, &$form_state) {
  $selfservice = $form_state['selfservice'];
  entity_form_submit_build_entity('borgerdk_selfservice', $selfservice, $form, $form_state);

  borgerdk_selfservice_save($selfservice);
  $uri = entity_uri('borgerdk_selfservice', $selfservice);

  $form_state['redirect'] = $uri['path'];
  drupal_set_message(t('Selfservice %title saved.', array('%title' => entity_label('borgerdk_selfservice', $selfservice))));
}

/**
 * Selfservice delete confirmation form.
 */
function bellcom_borgerdk_selfservice_delete_form($form, &$form_state, $selfservice) {
  $form_state['selfservice'] = $selfservice;

  // Always provide entity id in the same form key as in the entity edit form.
  $form['entity_id'] = array('#type' => 'value', '#value' => entity_id('borgerdk_selfservice', $selfservice));

  $entity_uri = entity_uri('borgerdk_selfservice', $selfservice);
  return confirm_form($form,
    t('Are you sure you want to delete Borger.dk self-service %selfservice?', array('%selfservice' => entity_label('borgerdk_selfservice', $selfservice))),
    $entity_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Selfservice delete form submit handler.
 */
function bellcom_borgerdk_selfservice_delete_form_submit($form, &$form_state) {
  $selfservice = $form_state['selfservice'];
  borgerdk_selfservice_delete($selfservice);

  drupal_set_message(t('Borger.dk self-service %title deleted.', array('%title' => entity_label('borgerdk_selfservice', $selfservice))));

  $form_state['redirect'] = '<front>';
}
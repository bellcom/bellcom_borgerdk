<?php

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
    t('Deleting microarticle will orphan all dependant self-services') . '<br/><br/>' . t('This action cannot be undone.'),
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
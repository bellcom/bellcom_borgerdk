<?php
/**
 * Self-service create/edit form
 *
 * @param $form
 * @param $form_state
 * @param null $selfservice
 * @return mixed
 */
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
    '#required' => TRUE,
    '#size' => 50,
  );

  $form['url'] = array(
    '#title' => t('URL'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#size' => 100,
    '#maxlength' => 255,
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
    $entities = borgerdk_microarticle_load_multiple_sorted(FALSE, array('article_id' => $article_entity_id), TRUE);
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

/**
 * Generates an overview table of older revisions of a selfservice.
 *
 * @param $selfservice
 *   A selfservice entity object.
 *
 * @return array
 *   An array as expected by drupal_render().
 */
function bellcom_borgerdk_selfservice_revision_overview($selfservice) {
  drupal_set_title(t('Revisions for %title', array('%title' => $selfservice->title)), PASS_THROUGH);

  $header = array(t('Revision'), array('data' => t('Operations'), 'colspan' => 2));

  $revisions = $selfservice->getRevisionList();

  $rows = array();
  $revert_permission = FALSE;
  if ((user_access('administer borgerdk_article')) && entity_access('update', 'borgerdk_selfservice', $selfservice)) {
    $revert_permission = TRUE;
  }
  $delete_permission = FALSE;
  if ((user_access('administer borgerdk_article')) && entity_access('delete', 'borgerdk_selfservice', $selfservice)) {
    $delete_permission = TRUE;
  }
  foreach ($revisions as $revision) {
    $row = array();
    $operations = array();

    $entity_path = entity_uri('borgerdk_selfservice', $selfservice)['path'];
    if ($revision->current_vid > 0) {
      $row[] = array(
        'data' => t('!date by !username', array(
          '!date' => l(format_date($revision->changed, 'short'), $entity_path),
          '!username' => ($revision->uid == 0) ? 'Borger.dk' : theme('username', array('account' => $revision))
        )),
        'class' => array('revision-current')
      );
      $operations[] = array(
        'data' => drupal_placeholder(t('current revision')),
        'class' => array('revision-current'),
        'colspan' => 2
      );
    }
    else {
      $row[] = t('!date by !username', array(
        '!date' => l(format_date($revision->changed, 'short'), "$entity_path/revisions/$revision->vid/view"),
        '!username' => ($revision->uid == 0) ? 'Borger.dk' : theme('username', array('account' => $revision))
      ));
      if ($revert_permission) {
        $operations[] = l(t('revert'), "$entity_path/revisions/$revision->vid/revert");
      }
      if ($delete_permission) {
        $operations[] = l(t('delete'), "$entity_path/revisions/$revision->vid/delete");
      }
    }
    $rows[] = array_merge($row, $operations);
  }

  $build['borgerdk_selfservice_revisions_table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#header' => $header,
  );

  return $build;
}

/**
 * Asks for confirmation of the reversion to prevent against CSRF attacks.
 *
 * @param int $selfservice_revision
 *   The selfservice revision ID.
 *
 * @return array
 *   An array as expected by drupal_render().
 *
 * @see node_menu()
 * @see node_revision_revert_confirm_submit()
 * @ingroup forms
 */
function bellcom_borgerdk_selfservice_revision_revert_confirm($form, $form_state, $selfservice_revision) {
  $form['#selfservice_revision'] = $selfservice_revision;
  $entity_path = entity_uri('borgerdk_selfservice', $selfservice_revision)['path'];
  return confirm_form($form, t('Are you sure you want to revert to the revision from %revision-date?', array('%revision-date' => format_date($selfservice_revision->changed))), "$entity_path/revisions", '', t('Revert'), t('Cancel'));
}

/**
 * Form submission handler for bellcom_borgerdk_selfservice_revision_revert_confirm().
 */
function bellcom_borgerdk_selfservice_revision_revert_confirm_submit($form, &$form_state) {
  $selfservice_revision = $form['#selfservice_revision'];

  //needed to use this as default revision
  $selfservice_revision->default_revision = 1;
  //we don't want another revision entry created
  $selfservice_revision->is_new_revision = FALSE;

  $revision_timestamp = $selfservice_revision->changed;
  borgerdk_selfservice_save($selfservice_revision);

  watchdog('content', 'Borger.dk selfservice: reverted %title revision %revision.', array(
    '%title' => $selfservice_revision->title,
    '%revision' => $selfservice_revision->vid
  ));
  drupal_set_message(t('Borger.dk selfservice %title has been reverted back to the revision from %revision-date.', array(
    '%title' => $selfservice_revision->title,
    '%revision-date' => format_date($revision_timestamp)
  )));
  $entity_path = entity_uri('borgerdk_selfservice', $selfservice_revision)['path'];
  $form_state['redirect'] = "$entity_path/revisions";
}

/**
 * Form constructor for the revision deletion confirmation form.
 *
 * This form prevents against CSRF attacks.
 *
 * @param $node_revision
 *   The node revision ID.
 *
 * @return
 *   An array as expected by drupal_render().
 *
 * @see bellcom_borgerdk_selfservice_revision_delete_confirm_submit()
 * @ingroup forms
 */
function bellcom_borgerdk_selfservice_revision_delete_confirm($form, $form_state, $selfservice_revision) {
  $form['#selfservice_revision'] = $selfservice_revision;
  $entity_path = entity_uri('borgerdk_selfservice', $selfservice_revision)['path'];
  return confirm_form($form, t('Are you sure you want to delete the revision from %revision-date?', array('%revision-date' => format_date($selfservice_revision->changed))), "$entity_path/revisions", t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Form submission handler for bellcom_borgerdk_selfservice_revision_delete_confirm().
 */
function bellcom_borgerdk_selfservice_revision_delete_confirm_submit($form, &$form_state) {
  $selfservice_revision = $form['#selfservice_revision'];
  borgerdk_selfservice_delete_revision($selfservice_revision->vid);

  watchdog('content', 'Borger.dk selfservice deleted %title revision %revision.', array(
    '%title' => $selfservice_revision->title,
    '%revision' => $selfservice_revision->vid
  ));
  drupal_set_message(t('Revision from %revision-date of Borger.dk selfservice  %title has been deleted.', array(
    '%revision-date' => format_date($selfservice_revision->changed),
    '%title' => $selfservice_revision->title
  )));
  $entity_path = entity_uri('borgerdk_selfservice', $selfservice_revision)['path'];
  $form_state['redirect'] = $entity_path;

  if (db_query('SELECT COUNT(vid) FROM {borgerdk_selfservice_revision} WHERE entity_id = :entity_id', array(':entity_id' => $selfservice_revision->entity_id))->fetchField() > 1) {
    $form_state['redirect'] .= '/revisions';
  }
}
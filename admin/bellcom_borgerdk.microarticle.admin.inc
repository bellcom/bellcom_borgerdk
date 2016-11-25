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

/**
 * Generates an overview table of older revisions of a microarticle.
 *
 * @param $microarticle
 *   A microarticle entity object.
 *
 * @return array
 *   An array as expected by drupal_render().
 */
function bellcom_borgerdk_microarticle_revision_overview($microarticle) {
  drupal_set_title(t('Revisions for %title', array('%title' => $microarticle->title)), PASS_THROUGH);

  $header = array(t('Revision'), array('data' => t('Operations'), 'colspan' => 2));

  $revisions = $microarticle->getRevisionList();

  $rows = array();
  $revert_permission = FALSE;
  if ((user_access('administer borgerdk_article')) && entity_access('update', 'borgerdk_microarticle', $microarticle)) {
    $revert_permission = TRUE;
  }
  $delete_permission = FALSE;
  if ((user_access('administer borgerdk_article')) && entity_access('delete', 'borgerdk_microarticle', $microarticle)) {
    $delete_permission = TRUE;
  }
  foreach ($revisions as $revision) {
    $row = array();
    $operations = array();

    $entity_path = entity_uri('borgerdk_microarticle', $microarticle)['path'];
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

  $build['borgerdk_microarticle_revisions_table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#header' => $header,
  );

  return $build;
}

/**
 * Asks for confirmation of the reversion to prevent against CSRF attacks.
 *
 * @param int $microarticle_revision
 *   The microarticle revision ID.
 *
 * @return array
 *   An array as expected by drupal_render().
 *
 * @see node_menu()
 * @see node_revision_revert_confirm_submit()
 * @ingroup forms
 */
function bellcom_borgerdk_microarticle_revision_revert_confirm($form, $form_state, $microarticle_revision) {
  $form['#microarticle_revision'] = $microarticle_revision;
  $entity_path = entity_uri('borgerdk_microarticle', $microarticle_revision)['path'];
  return confirm_form($form, t('Are you sure you want to revert to the revision from %revision-date?', array('%revision-date' => format_date($microarticle_revision->changed))), "$entity_path/revisions", '', t('Revert'), t('Cancel'));
}

/**
 * Form submission handler for bellcom_borgerdk_microarticle_revision_revert_confirm().
 */
function bellcom_borgerdk_microarticle_revision_revert_confirm_submit($form, &$form_state) {
  $microarticle_revision = $form['#microarticle_revision'];

  //needed to use this as default revision
  $microarticle_revision->default_revision = 1;
  //we don't want another revision entry created
  $microarticle_revision->is_new_revision = FALSE;

  $revision_timestamp = $microarticle_revision->changed;
  borgerdk_microarticle_save($microarticle_revision);

  watchdog('content', 'Borger.dk Microarticle: reverted %title revision %revision.', array(
    '%title' => $microarticle_revision->title,
    '%revision' => $microarticle_revision->vid
  ));
  drupal_set_message(t('Borger.dk Microarticle %title has been reverted back to the revision from %revision-date.', array(
    '%title' => $microarticle_revision->title,
    '%revision-date' => format_date($revision_timestamp)
  )));
  $entity_path = entity_uri('borgerdk_microarticle', $microarticle_revision)['path'];
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
 * @see bellcom_borgerdk_microarticle_revision_delete_confirm_submit()
 * @ingroup forms
 */
function bellcom_borgerdk_microarticle_revision_delete_confirm($form, $form_state, $microarticle_revision) {
  $form['#microarticle_revision'] = $microarticle_revision;
  $entity_path = entity_uri('borgerdk_microarticle', $microarticle_revision)['path'];
  return confirm_form($form, t('Are you sure you want to delete the revision from %revision-date?', array('%revision-date' => format_date($microarticle_revision->changed))), "$entity_path/revisions", t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Form submission handler for bellcom_borgerdk_microarticle_revision_delete_confirm().
 */
function bellcom_borgerdk_microarticle_revision_delete_confirm_submit($form, &$form_state) {
  $microarticle_revision = $form['#microarticle_revision'];
  borgerdk_microarticle_delete_revision($microarticle_revision->vid);

  watchdog('content', 'Borger.dk microarticle deleted %title revision %revision.', array(
    '%title' => $microarticle_revision->title,
    '%revision' => $microarticle_revision->vid
  ));
  drupal_set_message(t('Revision from %revision-date of Borger.dk microarticle  %title has been deleted.', array(
    '%revision-date' => format_date($microarticle_revision->changed),
    '%title' => $microarticle_revision->title
  )));
  $entity_path = entity_uri('borgerdk_microarticle', $microarticle_revision)['path'];
  $form_state['redirect'] = $entity_path;

  if (db_query('SELECT COUNT(vid) FROM {borgerdk_microarticle_revision} WHERE entity_id = :entity_id', array(':entity_id' => $microarticle_revision->entity_id))->fetchField() > 1) {
    $form_state['redirect'] .= '/revisions';
  }
}

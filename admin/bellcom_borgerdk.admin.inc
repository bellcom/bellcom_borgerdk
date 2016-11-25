<?php

/**
 * Implementation of settings form specifically for Bellcom Borger.dk integration.
 *
 * @param $form       the settings form
 * @param $form_state the state of the form
 *
 * @return settings form.
 */
function bellcom_borgerdk_settings_form($form, $form_state) {
  $form['bellcom_borgerdk_synch'] = array(
    '#type' => 'fieldset',
    '#title' => t('Synch settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['bellcom_borgerdk_synch']['bellcom_borgerdk_selected_municipality'] = array(
    '#type' => 'select',
    '#title' => t('Fetch content for selected municipality'),
    '#options' => bellcom_borgedk_get_municipalities_list(),
    '#required' => FALSE,
    '#empty_value' => 0,
    '#description' => t('Select "- None -" to not have the specific municipality content'),
    '#default_value' => variable_get('bellcom_borgerdk_selected_municipality', 0),
  );
  $form['#submit'][] = 'bellcom_borgerdk_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Custom submit form bellcom_borgerdk_settings_form behaviour.
 *
 * @param $form
 * @param $form_state
 */
function bellcom_borgerdk_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];

  //if selected municipality changed = schedule all articles for resynch
  if (variable_get('bellcom_borgerdk_selected_municipality') != $values['bellcom_borgerdk_selected_municipality']) {
    $articles_to_resynch = borgerdk_article_load_multiple(FALSE, array('resynch' => 0));
    foreach ($articles_to_resynch as $article) {
      $article->scheduleForResynch();
    }
  }
}

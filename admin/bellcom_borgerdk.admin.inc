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

  $form['bellcom_borgerdk_reminder'] = array(
    '#type' => 'fieldset',
    '#title' => t('Obsolete articles reminder settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['bellcom_borgerdk_reminder']['bellcom_borgerdk_reminder_email_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send notification about obsotele Borger.dk articles'),
    '#default_value' => variable_get('bellcom_borgerdk_reminder_email_enabled', FALSE),
    '#description' => t('Comma-separated list of committees changing status of which should send an email notification.'),
  );
  $form['bellcom_borgerdk_reminder']['bellcom_borgerdk_reminder_email_recepient'] = array(
    '#type' => 'textfield',
    '#title' => t('Recepient(s) of the email'),
    '#description' => t('email@example.com, anotheremail@test.com'),
    '#default_value' => variable_get('bellcom_borgerdk_reminder_email_recepient'),
    '#states' => array(
      'visible' => array(
        ':input[name="bellcom_borgerdk_reminder_email_enabled"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['bellcom_borgerdk_reminder']['bellcom_borgerdk_reminder_email_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Email subject'),
    '#default_value' => variable_get('bellcom_borgerdk_reminder_email_subject'),
    '#states' => array(
      'visible' => array(
        ':input[name="bellcom_borgerdk_reminder_email_enabled"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['bellcom_borgerdk_reminder']['bellcom_borgerdk_reminder_email_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Email text'),
    '#default_value' => variable_get('bellcom_borgerdk_reminder_email_body'),
    '#states' => array(
      'visible' => array(
        ':input[name="bellcom_borgerdk_reminder_email_enabled"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['bellcom_borgerdk_reminder']['bellcom_borgerdk_reminder_email_variables'] = array(
    '#title' => t('Available variables'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => t('Can be used both in subject and body:') . '<br/>
    <b>!article_title</b> - ' . t('title of the article') . '<br/>
    <b>!nodes</b> - ' . t('list of used referencing this article') . '<br/>',
    '#states' => array(
      'visible' => array(
        ':input[name="bellcom_borgerdk_reminder_email_enabled"]' => array('checked' => TRUE),
      ),
    ),
  );

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
    $articles_to_resynch = borgerdk_article_load_multiple(FALSE, array('resynch' => 0, 'uid' => 0));
    foreach ($articles_to_resynch as $article) {
      $article->scheduleForResynch();
    }
  }
}

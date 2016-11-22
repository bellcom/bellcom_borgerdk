<?php

/**
 * Article create/edit form
 *
 * @param $form
 * @param $form_state
 * @param null $article
 * @return array
 */
function borgerdk_article_form($form, &$form_state, $article = NULL) {
  $form[] = array(
    '#markup' => t('Borger.dk article editing/creating is not supported')
  );
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
    t('Deleting article will also delete all dependant microarticles and self-services') . '<br/><br/>' . t('This action cannot be undone.'),
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
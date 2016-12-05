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
  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#size' => 50,
  );

  $form['header'] = array(
    '#title' => t('Header'),
    '#type' => 'textfield',
    '#size' => 50,
  );

  $form['legislation'] = array(
    '#title' => t('Legislation'),
    '#type' => 'text_format',
    '#format' => 'full_html',
  );

  $form['recommendation'] = array(
    '#title' => t('Recommendation'),
    '#type' => 'text_format',
    '#format' => 'full_html',
  );

  $form['byline'] = array(
    '#title' => t('Byline'),
    '#type' => 'textfield',
    '#size' => 50,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 40,
  );

  //loading default values or creating new entity
  if ($article && !empty($article->entity_id)) {
    $form['title']['#default_value'] = $article->title;
    $form['header']['#default_value'] = $article->header;
    $form['legislation']['#default_value'] = $article->legislation;
    $form['recommendation']['#default_value'] = $article->recommendation;
    $form['byline']['#default_value'] = $article->byline;
  }
  else {
    $article = entity_create('borgerdk_article', array('resynch' => FALSE));
  }
  $form_state['article'] = $article;

  return $form;
}

/**
 * Submit handler for creating/editing article.
 */
function borgerdk_article_form_submit(&$form, &$form_state) {
  $article = $form_state['article'];
  entity_form_submit_build_entity('borgerdk_article', $article, $form, $form_state);

  $article->legislation = $article->legislation['value'];
  $article->recommendation = $article->recommendation['value'];
  $article->lastUpdated = REQUEST_TIME;

  borgerdk_article_save($article);
  $uri = entity_uri('borgerdk_article', $article);

  $form_state['redirect'] = $uri['path'];
  drupal_set_message(t('Article %title saved.', array('%title' => entity_label('borgerdk_article', $article))));
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
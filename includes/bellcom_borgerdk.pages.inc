<?php

/**
 * Borger.dk article view callback.
 */
function bellcom_borgerdk_article_view($borgerdk_article) {
  drupal_set_title(entity_label('borgerdk_article', $borgerdk_article));
  return entity_view('borgerdk_article', array(entity_id('borgerdk_article', $borgerdk_article) => $borgerdk_article), 'basic_info');
}

/**
 * Borger.dk microarticle view callback.
 */
function bellcom_borgerdk_microarticle_view($borgerdk_microarticle, $revision = false) {
  drupal_set_title(entity_label('borgerdk_microarticle', $borgerdk_microarticle));
  if ($revision) {
    drupal_set_title(t('Revision of %title from %date', array('%title' => $borgerdk_microarticle->title, '%date' => format_date($borgerdk_microarticle->changed))), PASS_THROUGH);
  }
  return entity_view('borgerdk_microarticle', array(entity_id('borgerdk_microarticle', $borgerdk_microarticle) => $borgerdk_microarticle), 'full');
}

/**
 * Borger.dk self-service view callback.
 */
function bellcom_borgerdk_selfservice_view($borgerdk_selfservice, $revision = false) {
  drupal_set_title(entity_label('borgerdk_selfservice', $borgerdk_selfservice));
  if ($revision) {
    drupal_set_title(t('Revision of %title from %date', array('%title' => $borgerdk_selfservice->title, '%date' => format_date($borgerdk_selfservice->changed))), PASS_THROUGH);
  }
  return entity_view('borgerdk_selfservice', array(entity_id('borgerdk_selfservice', $borgerdk_selfservice) => $borgerdk_selfservice), 'full');
}

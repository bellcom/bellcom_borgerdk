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
function bellcom_borgerdk_microarticle_view($borgerdk_microarticle) {
  drupal_set_title(entity_label('borgerdk_microarticle', $borgerdk_microarticle));
  return entity_view('borgerdk_microarticle', array(entity_id('borgerdk_microarticle', $borgerdk_microarticle) => $borgerdk_microarticle), 'full');
}

/**
 * Borger.dk self-service view callback.
 */
function bellcom_borgerdk_selfservice_view($borgerdk_selfservice) {
  drupal_set_title(entity_label('borgerdk_selfservice', $borgerdk_selfservice));
  return entity_view('borgerdk_selfservice', array(entity_id('borgerdk_selfservice', $borgerdk_selfservice) => $borgerdk_selfservice), 'full');
}

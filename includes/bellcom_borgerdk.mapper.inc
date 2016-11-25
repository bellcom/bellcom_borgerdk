<?php

/**
 * Mapps the information from SOAP returned content - Article with provided entity - borgerdk_article
 *
 * @param $entity_article
 * @param $soap_article
 * @return borgerdk_article with updated values
 */
function bellcom_borgerdk_article_map($entity_article, $soap_article) {
  $entity_article->entity_id = $soap_article->id;
  $entity_article->title = $soap_article->title;
  $entity_article->articleUrl = $soap_article->url;
  $entity_article->lastUpdated = strtotime($soap_article->lastUpdated);
  $entity_article->publishingDate = strtotime($soap_article->publishDate);
  if (isset($soap_article->header)) {
    $entity_article->header = $soap_article->header;
  }
  if (isset($soap_article->legislation)) {
    $entity_article->legislation =$soap_article->legislation->content;
  }
  if (isset($soap_article->recommendation)) {
    $entity_article->recommendation =$soap_article->recommendation->content;
  }
  if (isset($soap_article->byline)) {
    $entity_article->byline =$soap_article->byline;
  }

  return $entity_article;
}

/**
 * Maps the information the SOAP returned content - Microarticle with provided entity - borgerdk_microarticle
 *
 * @param $entity_microarticle
 * @param $soap_microarticle
 * @return borgerdk_microarticle with updated values
 */
function bellcom_borgerdk_microarticle_map($entity_microarticle, $soap_microarticle) {
  $entity_microarticle->entity_id = $soap_microarticle->id;
  $entity_microarticle->title = $soap_microarticle->headline;
  $entity_microarticle->content = $soap_microarticle->content;

  return $entity_microarticle;
}

/**
 * Maps the information the SOAP returned content - Selfservice with provided entity - borgerdk_selfservice
 *
 * @param $entity_selfservice
 * @param $soap_selfservice
 * @return borgerdk_selfservice with updated values
 */
function bellcom_borgerdk_selfservice_map($entity_selfservice, $soap_selfservice) {
  $entity_selfservice->entity_id = $soap_selfservice->id;
  $entity_selfservice->title = $soap_selfservice->title;
  $entity_selfservice->label = $soap_selfservice->label;
  $entity_selfservice->url = $soap_selfservice->url;

  return $entity_selfservice;
}
 
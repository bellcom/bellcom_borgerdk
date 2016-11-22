<?php

abstract class BorgerdkAbstractEntityController extends EntityAPIController {

  public function create(array $values = array()) {
    $values += array(
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'title' => '',
    );
    return parent::create($values);
  }

  public function save($entity) {
    $entity->changed = REQUEST_TIME;
    return parent::save($entity);
  }

  /**
   * Generates an unique random entity ID
   *
   * @param $entity
   * @return string
   */
  protected function generateEntityId($entity) {
    $hash = hash('md5', $entity->article_id . time());
    $entity_id = substr($hash, 0, 8) . '-' . substr($hash, 8, 4) . '-' . substr($hash, 12, 4) . '-' . substr($hash, 16, 4) . '-' . substr($hash, 20, 12);
    if (!$this->isEntityIdUnique($entity_id)) {
      $entity_id = $this->generateEntityId($entity);
    }
    return $entity_id;
  }

  /**
   * Checks if a provided entity id is in fact unique
   *
   * @param $entity_id
   * @return bool
   */
  protected function isEntityIdUnique($entity_id) {
    $entities = $this->load(array($entity_id));
    return empty($entities);
  }
}
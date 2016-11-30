<?php

abstract class BorgerdkAbstractEntityController extends EntityAPIController {

  /**
   * Sets created, changed time to the time of request, and delegaes to parent create function
   *
   * @param array $values
   * @return object
   */
  public function create(array $values = array()) {
    $values += array(
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'title' => '',
    );
    return parent::create($values);
  }

  /**
   * Updates the entity changed time, and delegates to part save function
   *
   * @param $entity
   * @param DatabaseTransaction $transaction
   * @return bool|int
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    //update the changed time only if we don't have versioning
    //or if the operation is in fact creating the new revision.
    //this helps to avoid situation where changed time is changed during revision revert
    if (!isset($entity->is_new_revision) || (isset($entity->is_new_revision) && $entity->is_new_revision)) {
      $entity->changed = REQUEST_TIME;
    }
    return parent::save($entity, $transaction);
  }

  /**
   * Generates an unique random entity ID
   *
   * @param $entity
   * @return string
   */
  protected function generateEntityId($entity) {
    $hash = hash('md5', $entity->article_id . time());
    $entity_id = substr($hash, 0, 8) . '-' . substr($hash, 8, 4) . '-' . substr($hash, 12, 4) . '-' . substr($hash, 16, 4) . '-' . substr($hash, 20, 12) . '-' . $entity->article_id;
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
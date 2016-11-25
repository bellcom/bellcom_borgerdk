<?php
/**
 * BorgerdkSelfserviceEntity class.
 */
class BorgerdkSelfserviceEntity extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'borgerdk/selfservice/' . $this->identifier());
  }

  /**
   * Returns a list of revisions for this entity.
   *
   * @return array of revisions mapped by revision id
   */
  public function getRevisionList() {
    $revisions = array();
    $result = db_query('
    SELECT r.vid, r.title, r.uid, s.vid AS current_vid, r.changed, u.name
    FROM {borgerdk_selfservice_revision} r
    LEFT JOIN {borgerdk_selfservice} s ON s.vid = r.vid INNER JOIN {users} u ON u.uid = r.uid WHERE r.entity_id = :entity_id
    ORDER BY r.vid DESC', array(':entity_id' => $this->entity_id));

    foreach ($result as $revision) {
      $revisions[$revision->vid] = $revision;
    }

    return $revisions;
  }
}
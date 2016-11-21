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
}
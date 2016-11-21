<?php
/**
 * BorgerdkMicroarticleEntity class.
 */
class BorgerdkMicroarticleEntity extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'borgerdk/microarticle/' . $this->identifier());
  }
}
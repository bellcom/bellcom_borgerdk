<?php
/**
 * BorgerdkArticleEntity class.
 */
class BorgerdkArticleEntity extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'borgerdk/article/' . $this->identifier());
  }
}
<?php

/**
 * BorgerdkArticleEntity class.
 */
class BorgerdkArticleEntity extends Entity {
  /**
   * Overwrites parents defaultLabel function.
   * Returns entity label.
   *
   * @return array|bool|string
   */
  protected function defaultLabel() {
    return $this->title;
  }

  /**
   * Overwrites parents defaultUri function.
   * Returns a correct path to an entity.
   *
   * @return array
   */
  protected function defaultUri() {
    return array('path' => 'borgerdk/article/' . $this->identifier());
  }
}
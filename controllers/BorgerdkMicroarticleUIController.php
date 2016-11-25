<?php

/**
 * Custom controller for the administrator UI: Microarticle
 */
class BorgerdkMicroarticleUIController extends EntityDefaultUIController {
  /**
   * Override the menu hook for default ui controller.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['title'] = t('Borger.dk Microarticles');
    $items[$this->path]['description'] = t('Manage Borger.dk Microarticles, including fields.');
    $items[$this->path]['access callback'] = 'user_access';
    $items[$this->path]['access arguments'] = array('administer borgerdk_article');
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    return $items;
  }

  /**
   * Admin form for searching and doing bulk operations.
   */
  public function overviewForm($form, &$form_state) {
    $header = array(
      'title' => array('data' => t('Title'), 'type' => 'property', 'specifier' => 'title', 'sort' => 'asc'),
      'content' => array('data' => t('Content')),
      'articleId' => array('data' => t('Article ID'), 'type' => 'property', 'specifier' => 'article_id'),
      'selfservices' => array('data' => t('Self-services #')),
      'author' => array('data' => t('Author'), 'type' => 'property', 'specifier' => 'uid'),
      'edit' => array('data' => t('Edit')),
      'delete' => array('data' => t('Delete')),
    );

    $options = array();

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'borgerdk_microarticle');
    $query->tableSort($header);

    $search_params = array();
    if (!empty($_GET['title'])) {
      $search_params['title'] = $_GET['title'];
      $query->propertyCondition('title', '%' . $search_params['title'] . '%', 'like');
    }
    if (!empty($_GET['article_id'])) {
      $search_params['article_id'] = $_GET['article_id'];
      $query->propertyCondition('article_id', $search_params['article_id']);
    }

    $query->pager(BELLCOM_BORGERDK_CONTROLLER_UI_PAGER_LIMIT);
    $result = $query->execute();

    $borgerdk_microarticle_results = !empty($result['borgerdk_microarticle']) ? $result['borgerdk_microarticle'] : array();
    $borgerdk_microarticle_array = !empty($borgerdk_microarticle_results) ? borgerdk_microarticle_load_multiple(array_keys($borgerdk_microarticle_results)) : array();
    foreach ($borgerdk_microarticle_array as $entity_id => $ma) {
      $article = borgerdk_article_load($ma->article_id);
      $strip_content = strip_tags($ma->content);
      $selfservices_count = count(borgerdk_selfservice_load_multiple(FALSE, array('microarticle_id' => $entity_id)));
      $author = user_load($ma->uid);

      $entity_path = entity_uri('borgerdk_microarticle', $ma)['path'];

      $options[$entity_id] = array(
        'title' => l($ma->title, $entity_path),
        'content' => mb_substr($strip_content, 0, 50) . ((mb_strlen($strip_content) > 50) ? '...' : ''),
        'articleId' => l($article->entity_id, entity_uri('borgerdk_article', $article)['path']),
        'selfservices' => $selfservices_count,
        'author' => ($author->uid) ? theme('username', array('account' => $author)) : 'Borger.dk',
        'edit' =>
          l(t('Edit'), "$entity_path/edit", array('query' => array('destination' => entity_get_info('borgerdk_microarticle')['admin ui']['path']))),
        'delete' =>
          l(t('Delete'), "$entity_path/delete", array('query' => array('destination' => entity_get_info('borgerdk_microarticle')['admin ui']['path']))),
      );
    }

    $form['search'] = array(
      '#type' => 'fieldset',
      '#title' => t('Basic Search'),
      '#collapsible' => TRUE,
      '#collapsed' => !empty($search_term) ? FALSE : TRUE,
    );

    $form['search']['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#default_value' => !empty($search_params['title']) ? $search_params['title'] : '',
    );

    $article_options = bellcom_borgerdk_field_get_articles_options();
    $form['search']['article_id'] = array(
      '#title' => t('Filter by Borger.dk article'),
      '#type' => 'select',
      '#options' => $article_options,
      '#default_value' => !empty($_GET['article_id']) ? $_GET['article_id'] : 0,
      '#required' => FALSE,
      '#empty_value' => 0
    );

    $form['search']['search_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Search'),
    );

    $form['entities'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#attributes' => array('class' => array('entity-sort-table')),
      '#empty' => t('No content.'),
    );

    $form['pager'] = array('#theme' => 'pager');

    return $form;
  }

  /**
   * Form Submit method.
   */
  public function overviewFormSubmit($form, &$form_state) {
    $values = $form_state['input'];

    drupal_goto(entity_get_info('borgerdk_microarticle')['admin ui']['path'], array(
      'query' => array(
        'title' => $values['title'],
        'article_id' => $values['article_id']
      )
    ));
  }
}
<?php

/**
 * Custom controller for the administrator UI: Article
 */
class BorgerdkArticleUIController extends EntityDefaultUIController {
  /**
   * Override the menu hook for default ui controller.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['title'] = t('Borger.dk Articles');
    $items[$this->path]['description'] = t('Manage Borger.dk Articles, including fields.');
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
      'articleUrl' => array('data' => t('Article URL')),
      'publishingDate' => array('data' => t('Publishing Date'), 'type' => 'property', 'specifier' => 'publishingDate'),
      'lastUpdated' => array('data' => t('Last Updated'), 'type' => 'property', 'specifier' => 'lastUpdated'),
      //'operations' => array('data' => t('Operations')),
    );

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'borgerdk_article');
    $query->tableSort($header);

    $search_params = array();
    if (!empty($_GET['title'])) {
      $search_params['title'] = $_GET['title'];
      $query->propertyCondition('title', '%' . $search_params['title'] . '%', 'like');
    }

    $query->pager(BELLCOM_BORGERDK_CONTROLLER_UI_PAGER_LIMIT);
    $result = $query->execute();

    $borgerdk_article_results = !empty($result['borgerdk_article']) ? $result['borgerdk_article'] : array();
    $borgerdk_article_array = !empty($borgerdk_article_results) ? borgerdk_article_load_multiple(array_keys($borgerdk_article_results)) : array();

    $options = array();
    foreach ($borgerdk_article_array as $entity_id => $article) {
      $options[$entity_id] = array(
        'title' => l($article->title, entity_uri('borgerdk_article', $article)['path']),
        'articleUrl' => l($article->articleUrl, $article->articleUrl, array('attributes' => array('target' => '_blank'))),
        'publishingDate' => format_date($article->publishingDate),
        'lastUpdated' => format_date($article->lastUpdated),
        //'operations' => array(),
        //l(t('Edit'), ADMIN_CONTENT_LAWMAKERS_MANAGE_URI . $lawmakers_id, array('query' => array('destination' => ADMIN_CONTENT_LAWMAKERS_URI)))// . ' ' .
        //l(t('Delete'), ADMIN_CONTENT_LAWMAKERS_MANAGE_URI . $lawmakers_id . '/delete', array('attributes' => array('class' => array('lawmakers-delete-' . $lawmakers->lawmakers_id), ), 'query' => array('destination' => ADMIN_CONTENT_LAWMAKERS_URI))),
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

    drupal_goto(entity_get_info('borgerdk_article')['admin ui']['path'], array('query' => array('title' => $values['title'])));
  }
}
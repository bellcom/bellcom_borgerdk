<?php
/**
 * Custom controller for the administrator UI: Seflservice
 */
class BorgerdkSelfserviceUIController extends EntityDefaultUIController {
  /**
   * Override the menu hook for default ui controller.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['title'] = t('Borger.dk Selfservices');
    $items[$this->path]['description'] = t('Manage Borger.dk Selfservices, including fields.');
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
      'url' => array('data' => t('URL')),
      'articleId' => array('data' => t('Article ID'), 'type' => 'property', 'specifier' => 'article_id'),
      'microarticleId' => array('data' => t('Microarticle ID'), 'type' => 'property', 'specifier' => 'microarticle_id'),
      'author' => array('data' => t('Author'), 'type' => 'property', 'specifier' => 'uid'),
      'edit' => array('data' => t('Edit')),
      'delete' => array('data' => t('Delete')),
    );

    $options = array();

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'borgerdk_selfservice');
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

    $borgerdk_selfservice_results = !empty($result['borgerdk_selfservice']) ? $result['borgerdk_selfservice'] : array();
    $borgerdk_selfservice_array = !empty($borgerdk_selfservice_results) ? borgerdk_selfservice_load_multiple(array_keys($borgerdk_selfservice_results)) : array();
    foreach ($borgerdk_selfservice_array as $entity_id => $ss) {
      $article = borgerdk_article_load($ss->article_id);
      $microarticle = null;
      if ($ss->microarticle_id) {
        $microarticle = borgerdk_microarticle_load($ss->microarticle_id);
      }
      $author = user_load($ss->uid);

      $entity_path = entity_uri('borgerdk_selfservice', $ss)['path'];

      $options[$entity_id] = array(
        'title' => l($ss->title, $entity_path),
        'url' => l(mb_substr($ss->url, 0, 50) . '...' , $ss->url, array('attributes' => array('target'=>'_blank'))),
        'articleId' => l($article->entity_id, entity_uri('borgerdk_article', $article)['path']),
        'microarticleId' => ($microarticle)? l($microarticle->entity_id, entity_uri('borgerdk_microarticle', $microarticle)['path']) : '',
        'author' => ($author->uid) ? theme('username', array('account' => $author)) : 'Borger.dk',
        'edit' =>
          l(t('Edit'), "$entity_path/edit", array('query' => array('destination' => entity_get_info('borgerdk_selfservice')['admin ui']['path']))),
        'delete' =>
          l(t('Delete'), "$entity_path/delete", array('query' => array('destination' => entity_get_info('borgerdk_selfservice')['admin ui']['path']))),
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

    drupal_goto(entity_get_info('borgerdk_selfservice')['admin ui']['path'], array(
      'query' => array(
        'title' => $values['title'],
        'article_id' => $values['article_id']
      )
    ));
  }
} 
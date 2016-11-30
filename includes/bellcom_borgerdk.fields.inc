<?php
/**
 * Implements hook_field_info().
 */
function bellcom_borgerdk_field_info() {
  return array(
    'borgerdk_article_field' => array(
      'label' => t('Borger.dk article'),
      'description' => t('This field adds a connection to Borger.dk articles'),
      'default_widget' => 'borgerdk_article_widget',
      'default_formatter' => 'borgerdk_article_default_formatter',
    ),
  );
}

/**
 * Implements hook_field_widget_info().
 */
function bellcom_borgerdk_field_widget_info() {
  return array(
    'borgerdk_article_widget' => array(
      'label' => t('Borger.dk article'),
      'field types' => array('borgerdk_article_field'),
    ),
  );
}


function bellcom_borgerdk_field_settings_form($field, $instance, $has_data) {
  $settings = $field['settings'];
  // Add your global settings fields here
  $form = array();
  return $form;
}

/**
 * Implements hook_field_instance_settings_form().
 */
function bellcom_borgerdk_field_instance_settings_form($field, $instance) {
  $settings = $instance['settings'];
  // Add your instance settings fields here.
  $form = array();
  return $form;
}

/**
 * Implements hook_field_widget_form().
 */
function bellcom_borgerdk_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  switch ($instance['widget']['type']) {
    case 'borgerdk_article_widget' :

      $element['borgerdk_article_field'] = array(
        '#prefix' => '<div id="borgerdk-article-wrapper-' . $delta . '">',
        '#type' => 'fieldset',
        '#title' => t('Borger.dk article'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#suffix' => '</div>'
      );

      $article_options = bellcom_borgerdk_field_get_articles_options();
      $element['borgerdk_article_field']['borgerdk_article_entity_id'] = array(
        '#title' => t('Select Borger.dk article'),
        '#type' => 'select',
        '#options' => $article_options,
        '#default_value' => !empty($items[$delta]['borgerdk_article_entity_id']) ? $items[$delta]['borgerdk_article_entity_id'] : 0,
        '#ajax' => array(
          'callback' => 'bellcom_borgerdk_reload_article_info',
          'delta' => $delta
        ),
        '#required' => TRUE,
        '#empty_value' => -1
      );

      $element['borgerdk_article_field'][] = array(
        '#title' => 'Microarticles',
        '#theme' => 'form_element_label'
      );
      $element['borgerdk_article_field']['borgerdk_microarticle_entity_ids'] = array(
        '#type' => 'tableselect',
        '#header' => array(
          'title' => t('Microarticle title'),
          'content' => t('Content sample'),
          'selfservices' => ('Self-services #'),
          'author' => t('Author')
        ),
        '#options' => bellcom_borgerdk_field_get_microarticle_options($form_state),
        '#empty' => t('No content available.'),
        '#validated' => 'TRUE'
      );

      $element['borgerdk_article_field'][] = array(
        '#title' => 'Self-service links',
        '#theme' => 'form_element_label'
      );
      $element['borgerdk_article_field']['borgerdk_selfservice_entity_ids'] = array(
        '#type' => 'tableselect',
        '#header' => array('title' => t('Self-service title'), 'url' => t('Self-service URL'), 'author' => t('Author')),
        '#options' => bellcom_borgerdk_field_get_selfservice_options($form_state),
        '#empty' => t('No content available.'),
        '#validated' => 'TRUE'
      );


      if (isset($items[$delta]['borgerdk_article_entity_id'])) {
        $selected_article_entity_id = $items[$delta]['borgerdk_article_entity_id'];
        $selected_article_title = $article_options[$selected_article_entity_id];

        $element['borgerdk_article_field']['#title'] = t('Borger.dk article: @title', array('@title' => $selected_article_title));
        $element['borgerdk_article_field']['borgerdk_microarticle_entity_ids']['#options'] =
          bellcom_borgerdk_field_get_microarticle_options($form_state, $selected_article_entity_id);

        $element['borgerdk_article_field']['borgerdk_selfservice_entity_ids']['#options'] =
          bellcom_borgerdk_field_get_selfservice_options($form_state, $selected_article_entity_id);
      }

      if (isset($items[$delta]['borgerdk_microarticle_entity_ids'])) {
        $selected_microarticles = json_decode($items[$delta]['borgerdk_microarticle_entity_ids']);
        foreach ($selected_microarticles as $selected_ma) {
          $element['borgerdk_article_field']['borgerdk_microarticle_entity_ids']['#default_value'][$selected_ma] = 1;
        }
      }

      if (isset($items[$delta]['borgerdk_selfservice_entity_ids'])) {
        $selected_selfservice = json_decode($items[$delta]['borgerdk_selfservice_entity_ids']);
        foreach ($selected_selfservice as $selected_ss) {
          $element['borgerdk_article_field']['borgerdk_selfservice_entity_ids']['#default_value'][$selected_ss] = 1;
        }
      }

      break;
  }

  return $element;
}

/**
 * Borger.dk article dropdown select dropdown AJAX callback.
 * Reloads the article wrapper (changes the fieldset title, and reloads choice of microarticles and self-service links).
 *
 * @param $form
 * @param $form_state
 * @return array of ajax commands
 */
function bellcom_borgerdk_reload_article_info(&$form, &$form_state) {
  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#ajax']['delta'];

  $selected_article_entity_id = $form_state['triggering_element']['#value'];
  $selected_article_title = $form_state['triggering_element']['#options'][$selected_article_entity_id];

  //getting wrapper fieldset
  $wrapper = $form[$field_name]['und'][$delta]['borgerdk_article_field'];
  $wrapper['#title'] = t('Borger.dk article: @title', array('@title' => $selected_article_title));

  $form_state['rebuild'] = TRUE;
  return array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace("#borgerdk-article-wrapper-$delta", render($wrapper)),
    )
  );
}

/**
 * Implements hook_field_presave().
 */
function bellcom_borgerdk_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  foreach ($items as $delta => $item) {
    if (isset($item['borgerdk_article_field']['borgerdk_article_entity_id'])) {
      if ($item['borgerdk_article_field']['borgerdk_article_entity_id'] == -1) {
        //nothing is selected, delete lement with this delta
        unset($items[$delta]);
      }
      else {
        $items[$delta]['borgerdk_article_entity_id'] = $item['borgerdk_article_field']['borgerdk_article_entity_id'];

        //saving microarticles
        $selected_microarticles = array_filter($item['borgerdk_article_field']['borgerdk_microarticle_entity_ids']);
        $selected_microarticles = array_values($selected_microarticles);
        $items[$delta]['borgerdk_microarticle_entity_ids'] = json_encode($selected_microarticles);

        //saving self-services
        $selected_selfservices = array_filter($item['borgerdk_article_field']['borgerdk_selfservice_entity_ids']);
        $selected_selfservices = array_values($selected_selfservices);
        $items[$delta]['borgerdk_selfservice_entity_ids'] = json_encode($selected_selfservices);
      }
    }
  }
}

/**
 * Implements hook_field_is_empty().
 */
function bellcom_borgerdk_field_is_empty($item, $field) {
  if (empty($item['borgerdk_article_field']['borgerdk_article_entity_id'])) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Implements hook_field_validate().
 */
function bellcom_borgerdk_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  //no validation required
}

/**
 * Implements hook_field_formatter_info().
 */
function bellcom_borgerdk_field_formatter_info() {
  return array(
    'borgerdk_article_default_formatter' => array(
      'label' => t('Default'),
      'field types' => array('borgerdk_article_field'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function bellcom_borgerdk_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, &$display) {
  $element = array();
  switch ($display['type']) {
    case 'borgerdk_article_default_formatter' :
      foreach ($items as $delta => $item) {
        if (!empty($item['borgerdk_article_entity_id']) && !empty($item['borgerdk_microarticle_entity_ids'])) {
          $article = borgerdk_article_load($item['borgerdk_article_entity_id']);
          $element[$delta] = entity_view('borgerdk_article', array(entity_id('borgerdk_article', $article) => $article), 'full');

          //filtering microarticles, leaving only those selected
          $microarticle_ids = json_decode($item['borgerdk_microarticle_entity_ids']);
          if ($microarticle_ids) {
            foreach ($element[$delta]['borgerdk_article'][$article->entity_id]['microarticles']['#items'] as $id => $ma) {
              if (!in_array($id, $microarticle_ids)) {
                unset($element[$delta]['borgerdk_article'][$article->entity_id]['microarticles']['#items'][$id]);
                unset($element[$delta]['borgerdk_article'][$article->entity_id]['microarticles'][$id]);
              }
            }
          }
          else {
            //empty, removing field
            unset($element[$delta]['borgerdk_article'][$article->entity_id]['microarticles']);
          }

          //filtering selfservices, leaving only those selected
          $selfservice_ids = json_decode($item['borgerdk_selfservice_entity_ids']);
          if ($selfservice_ids) {
            foreach ($element[$delta]['borgerdk_article'][$article->entity_id]['selfservices']['#items'] as $id => $ma) {
              if (!in_array($id, $selfservice_ids)) {
                unset($element[$delta]['borgerdk_article'][$article->entity_id]['selfservices']['#items'][$id]);
                unset($element[$delta]['borgerdk_article'][$article->entity_id]['selfservices'][$id]);
              }
            }
          }
          else {
            //empty, removing field
            unset($element[$delta]['borgerdk_article'][$article->entity_id]['selfservices']);
          }
        }
      }
      break;
  }

  //making sure that the field label is not displayed inline, or it will break the entity formatting
  if ($display['label'] == 'inline') {
    $display['label'] = 'above';
  }

  return $element;
}

/**
 * Function to facilitate article dropdown select dropdown.
 *
 * @return mixed - array of Borger.dk articles
 */
function bellcom_borgerdk_field_get_articles_options() {
  $entities = borgerdk_article_load_multiple(FALSE, array(), TRUE);
  uasort($entities, function ($a, $b) {
    return strcmp($a->title, $b->title);
  });

  $options = array();
  foreach ($entities as $entity) {
    $options[$entity->entity_id] = decode_entities($entity->title) . ' (' . $entity->entity_id . ')';
  }

  return $options;
}

/**
 * Function to facilitate microarticle tableselect options.
 * In order to allow the ajax reloading of the options, function takes default $article_entity_id to load the options on page load
 * and form_state to dynamically extract triggering element from it.
 * Triggering element is Borger.dk article select dropdown.
 *
 * @param $form_state - current state of the form
 * @param null $article_entity_id - default value, used if form_state['triggering_element'] is empty
 * @return array - formatted array of select options
 */
function bellcom_borgerdk_field_get_microarticle_options($form_state, $article_entity_id = FALSE) {
  $options = array();

  //checking that trigger element is borgerdk_article_entity_id
  if (isset($form_state['triggering_element']) && array_pop($form_state['triggering_element']['#parents']) == 'borgerdk_article_entity_id') {
    $article_entity_id = $form_state['triggering_element']['#value'];
  }

  if ($article_entity_id) {
    //load microarticles
    $entities = borgerdk_microarticle_load_multiple_sorted(FALSE, array('article_id' => $article_entity_id), TRUE);

    foreach ($entities as $entity) {
      $strip_content = strip_tags($entity->content);
      $strip_content = mb_substr($strip_content, 0, 100) . ((mb_strlen($strip_content) > 100) ? '...' : '');
      $selfservices_count = count(borgerdk_selfservice_load_multiple(FALSE, array('microarticle_id' => $entity->entity_id)));
      $author = user_load($entity->uid);

      $options[$entity->entity_id] = array(
        'title' => $entity->title,
        'content' => $strip_content,
        'selfservices' => $selfservices_count,
        'author' => ($author->uid) ? theme('username', array('account' => $author)) : 'Borger.dk',
      );
    }
  }

  return $options;
}

/**
 * Function to facilitate selfservice tableselect options.
 * In order to allow the ajax reloading of the options, function takes default $article_entity_id to load the options on page load
 * and form_state to dynamically extract triggering element from it.
 * Triggering element is Borger.dk article select dropdown.
 *
 * @param $form_state - current state of the form
 * @param null $article_entity_id - default value, used if form_state['triggering_element'] is empty
 * @return array - formatted array of select options
 */
function bellcom_borgerdk_field_get_selfservice_options($form_state, $article_entity_id = FALSE) {
  $options = array();

  //checking that trigger element is borgerdk_article_entity_id
  if (isset($form_state['triggering_element']) && array_pop($form_state['triggering_element']['#parents']) == 'borgerdk_article_entity_id') {
    $article_entity_id = $form_state['triggering_element']['#value'];
  }

  if ($article_entity_id) {
    //load self-services
    $entities = borgerdk_selfservice_load_multiple_sorted(FALSE, array(
      'article_id' => $article_entity_id,
      'microarticle_id' => NULL
    ), TRUE);

    foreach ($entities as $entity) {
      $author = user_load($entity->uid);

      $options[$entity->entity_id] = array(
        'title' => $entity->title,
        'url' => $entity->url,
        'author' => ($author->uid) ? theme('username', array('account' => $author)) : 'Borger.dk',
      );
    }
  }

  return $options;
}
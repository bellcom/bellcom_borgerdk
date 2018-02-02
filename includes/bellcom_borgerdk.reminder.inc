<?php

/**
 * Returns the list of nodes, that are using this articles.
 *
 * @param $article
 * @return array
 */
function bellcom_borgerdk_reminder_get_affected_nodes($article) {
  $affected_nodes = array();

  $fields = field_read_fields(array('type' => 'borgerdk_article_field'));
  foreach ($fields as $field) {
    $field_instances = field_read_instances(array('field_id' => $field['id']));

    // Looping through fields to get affected nodes.
    foreach ($field_instances as $field_instance) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', $field_instance['entity_type'])
        ->entityCondition('bundle', $field_instance['bundle'])
        ->fieldCondition($field_instance['field_name'], 'borgerdk_article_entity_id', $article->entity_id);

      $result = $query->execute();
      if (isset($result['node'])) {
        $nids = array_keys($result['node']);
        $affected_nodes += node_load_multiple($nids);
      }
    }
  }

  return $affected_nodes;
}

/**
 * The actual function the sends the email with reminder
 * @param $article
 * @param $affected_nodes
 */
function bellcom_borgerdk_reminder_send_email($article, $affected_nodes) {
  global $base_url;
  global $base_path;

  // Getting subject and body.
  $subject_template = variable_get('bellcom_borgerdk_reminder_email_subject');
  $body_template = variable_get('bellcom_borgerdk_reminder_email_body');

  // Generating list of nodes.
  $affected_nodes_html = NULL;

  foreach ($affected_nodes as $affected_node) {
    //$affected_nodes_html = '<a href="' . url('node/' . $affected_node->nid, array('absolute' => TRUE)) . '">' . $affected_node->title . '</a><br/>';
    $affected_nodes_html .= $base_url . $base_path . 'node/' . $affected_node->nid;
  }

  // Composing search/replace.
  $search = array('!article_title', '!nodes', PHP_EOL);
  $replace = array($article->title, $affected_nodes_html, '<br/>');

  // Making replacements.
  $subject = str_replace($search, $replace, $subject_template);
  $subject = ucfirst($subject);
  $body = str_replace($search, $replace, $body_template);
  $body = ucfirst($body);

  $from = variable_get('system_mail');
  $params = array(
    'subject' => $subject,
    'body' => $body,
  );
  drupal_mail('bellcom_borgerdk', 'reminder_email', variable_get('bellcom_borgerdk_reminder_email_recepient'), language_default(), $params, $from);
}
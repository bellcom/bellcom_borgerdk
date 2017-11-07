<?php
/**
 * @author: Stanislav Kutasevits, stan@bellcom.dk
 *
 * This script fill the Borger.dk queue and force all elements to be processed.
 **/
$time_start = microtime(true);
print('==========================' . PHP_EOL);
print('Started bellcom_borgerdk.cron.php' . PHP_EOL);
print('==========================' . PHP_EOL);

print('Filling the queue' . PHP_EOL);
bellcom_borgerdk_cron();

$queue = DrupalQueue::get(BELLCOM_BORGERDK_QUEUE);
print('Total number of items in queue: ' . $queue->numberOfItems() * BELLCOM_BORGERDK_QUEUE_CHUNK_SIZE . PHP_EOL);
$total_chunks = $queue->numberOfItems();
$current_item = 1;
$startingNumber = $queue->numberOfItems();
while ($queue->numberOfItems() > 0 && $current_item <= $startingNumber) {
  print('Processing chunk : ' . $current_item . '/' . $total_chunks . PHP_EOL);
  $chunk = $queue->claimItem();
  if ($chunk) {
    $items = $chunk->data;
    if (is_array($items)) {
      try {
        print('Article IDs to process: ' . implode(', ', array_keys($chunk->data['items'])) . PHP_EOL);
        bellcom_borgerdk_queue_worker($chunk->data);
      } catch (Exception $e){
        print('Error! Ignoring chunk. Exception: ' . $e->getMessage());
      }
    }
    $queue->deleteItem($chunk);
  }
  $current_item++;
  print('==========================' . PHP_EOL);
}
print('==========================' . PHP_EOL);
print('Finished  bellcom_borgerdk.cron.php' . PHP_EOL);
print('Total execution time: ' . (microtime(true) - $time_start) . ' seconds' . PHP_EOL);
print('==========================' . PHP_EOL);
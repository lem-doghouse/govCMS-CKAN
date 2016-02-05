<?php
/**
 * @file
 * A temporary demonstration page. Will be removed once integration complete.
 */

/**
 * Demo page callback.
 */
function govcms_ckan_demo_page() {
  $output = array();
  $dataset = '531eb12d-4c60-4cdc-a26e-e500188c0d33';

  // Get a client and fetch response.
  $client = govcms_ckan_client();
  $response = $client->get('action/datastore_search', array('id' => $dataset));

  // Public URL.
  $public_url = variable_get('govcms_ckan_endpoint_url', '') . '/dataset/' . $dataset;

  // Page header.
  $output[] = '<p>Dataset: ' . $dataset . '</p>';
  $output[] = '<p><strong>' . l(t('> Go to dataset on data.gov.au'), $public_url) . '</strong></p>';

  // Get the Keys to use in the result set. These will be configurable via
  // admin but for now, hard coded for this dataset.
  $keys = _govcms_ckan_demo_get_keys();

  // Parse the records into tablular data.
  $table_set = array();
  $table_set['Style 1'] = _govcms_ckan_demo_parse_records($response->data->records, $keys, 'Taxa');
  $table_set['Style 2'] = _govcms_ckan_demo_parse_records_alt($response->data->records, $keys, 'Taxa');

  // Chart class.
  $chart_class = 'table-chart-demo';

  // Chart types.
  $chart_types = array('area', 'bar');

  // Present the data in two different formats depending on what should form the
  // lines vs axis labels.
  foreach ($table_set as $title => $tables) {

    // Add a title.
    $output[] = '<br /><h2>' . $title . '</h2><hr />';

    // Create a couple of chart types with the same data.
    foreach ($chart_types as $type) {
      // Attributes for chart usage.
      $table_defaults = array(
        'attributes' => array(
          'class' => array($chart_class),
          'data-type' => $type,
          'data-stacked' => 'true',
          'data-grid' => 'y',
          'data-xLabel' => 'Type',
          'data-yLabel' => 'Amount',
        ),
      );

      // This is RAW AS, definately not permanent.
      foreach ($tables as $year => $table_vars) {
        $output[] = '<div class="year-set" id="year-set-' . $year . '"><br />';
        $output[] = '<h3>' . $year . ' (' . ucwords($type) . ' chart)</h3>';
        $table_vars = array_merge($table_vars, $table_defaults);
        $output[] = theme('table', $table_vars);
        $output[] = '<br /></div>';
      }
    }

  }

  // Add the JS to the tables.
  govcms_ckan_display_add_charts('.' . $chart_class);

  return implode('', $output);
}

/**
 * Parse records so keys form the legend and label_keys form the x tick labels.
 *
 * @param array $record_set
 *   Records from the dataset.
 * @param array $keys
 *   The table keys.
 * @param string $label_key
 *   The Key that stores the labels.
 *
 * @return array
 *   Array of table vars (headers and rows) keyed by year.
 */
function _govcms_ckan_demo_parse_records($record_set, $keys = array(), $label_key = '') {
  $years = array();
  $tables = array();

  // Parse into years (A table for each year).
  foreach ($record_set as $record) {
    $years[$record->Year][] = $record;
  }

  // Make multiple tables.
  foreach ($years as $year => $records) {
    $rows = array();
    $header = array();

    // Add keys as header.
    foreach ($keys as $key) {
      if ($label_key == $key) {
        $header[] = array('data' => 'Type', 'data-xLabel' => 'true');
      }
      else {
        $header[] = $key;
      }
    }

    // Add records.
    foreach ($records as $record) {
      $row = array();

      foreach ($keys as $key) {
        if ($label_key == $key) {
          $row[] = array('data' => $record->{$key}, 'data-xLabel' => 'true');
        }
        else {
          $row[] = $record->{$key};
        }
      }

      // Add to rows array.
      $rows[] = $row;
    }

    $tables[$year] = array(
      'header' => $header,
      'rows' => $rows,
    );
  }

  return $tables;
}

/**
 * Parse the records so the label_key form the legend and keys are the x ticks.
 *
 * @param array $record_set
 *   Record dataset.
 * @param array $keys
 *   Keys to use.
 * @param string $label_key
 *   The Key that stores the labels.
 *
 * @return array
 *   Array of table vars (headers and rows) keyed by year.
 */
function _govcms_ckan_demo_parse_records_alt($record_set, $keys = array(), $label_key = '') {
  $years = array();
  $label_key = 'Taxa';
  $tables = array();

  // Parse into years (A table for each year).
  foreach ($record_set as $record) {
    $years[$record->Year][] = $record;
  }

  // Make multiple tables.
  foreach ($years as $year => $records) {
    $rows = array();
    $header = array(
      array('data' => 'Level', 'data-xLabel' => 'true'),
    );

    // Add records.
    foreach ($records as $col_num => $record) {
      // As we use the first column for the x tick labels, data cols all move
      // over by one column.
      $data_col_num = $col_num + 1;

      // Append to header.
      $header[] = $record->{$label_key};

      // Make the rows from the keys.
      foreach ($keys as $row_num => $key) {

        // Add all rows that are not the label key.
        if ($label_key != $key) {

          // Keys form the xLabel columns.
          if ($col_num == 0) {
            $rows[$row_num][0] = array('data' => $key, 'data-xLabel' => 'true');
          }

          // Add the data to the correct row/column.
          $rows[$row_num][$data_col_num] = $record->{$key};
        }
      }

    }

    $tables[$year] = array(
      'header' => $header,
      'rows' => $rows,
    );
  }

  return $tables;
}

/**
 * Get the keys for this dataset.
 */
function _govcms_ckan_demo_get_keys() {
  return array(
    'Taxa',
    'Endangered',
    'CriticallyEndangered',
    'ConservationDependent',
    'Vulnerable',
  );
}

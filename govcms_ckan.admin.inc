<?php

/**
 * @file
 * Code for the admin pages of the govCMS CKAN module.
 */

/**
 * Create settings form for govCMS CKAN.
 *
 * @return array
 *   Form API definition.
 */
function govcms_ckan_settings_form() {

  $form = array();

  $form['govcms_ckan_endpoint_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint Url'),
    '#description' => t('Specify the endpoint url. Example https://data.gov.au (please note no trailing slash)'),
    '#weight' => 0,
    '#size' => 100,
    '#required' => TRUE,
    '#default_value' => variable_get('govcms_ckan_endpoint_url', '')
  );

  $form['govcms_ckan_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Api Key'),
    '#description' => t('Specify the API key.'),
    '#weight' => 1,
    '#size' => 100,
    '#default_value' => variable_get('govcms_ckan_api_key', '')
  );

  $form['#validate'][] = 'govcms_ckan_settings_form_validate';

  return system_settings_form($form);
}

/**
 * Form validate handler for govCMS CKAN settings form.
 *
 * @see govcms_ckan_settings_form()
 */
function govcms_ckan_settings_form_validate($form, &$form_state) {
  $url = $form_state['values']['govcms_ckan_endpoint_url'] . '/api/3/action/package_list?limit=5';
  $response = drupal_http_request(check_url($url));

  $data = NULL;
  if (!empty($response->data)) {
    $data = json_decode($response->data);
  }

  if (!isset($data->success)) {
    form_set_error('govcms_ckan_endpoint_url', t('Could not establish a connection to the endpoint.'));
  }
}

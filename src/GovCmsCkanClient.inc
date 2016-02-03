<?php
/**
 * @file
 * GovCmsCkanClient Class for fetching, caching and returning ckan data.
 */

/**
 * Defines the GovCMS CKAN Client class.
 */
class GovCmsCkanClient {

  /**
   * API variables.
   */
  private $apiUrl = 'http://data.gov.au/api/3/';
  private $apiKey = '256a71bd-2f09-4a48-87bb-fb7af0cc51b7';

  /**
   * Request variables.
   */
  private $resource;
  private $id;
  private $idKey = 'id';
  private $url;

  /**
   * Response variable, gets updated whenever get/fetch is called.
   */
  private $response;

  /**
   * Cache table.
   */
  private $cacheTable = 'cache_govcms_ckan';

  /**
   * Constructor.
   *
   * Set up the fetch class.
   */
  public function __construct() {
    $this->getSettings();
  }

  /**
   * Return the data.
   *
   * @param string $resource
   *   Resource path.
   * @param string $id
   *   Id to request.
   * @param string $id_key
   *   Id Key for the resource.
   *
   * @return mixed
   *   Parsed response.
   */
  public function get($resource, $id, $id_key = 'id') {
    // Prepare the variables for the url.
    $this->resource = $resource;
    $this->id = $id;
    $this->idKey = $id_key;

    // Fetch the response.
    $this->fetch();

    return $this->response;
  }

  /**
   * Populate the settings using drupal variables.
   */
  private function getSettings() {

  }

  /**
   * Fetch CKAN data.
   */
  private function fetch() {
    $this->url = url($this->apiUrl . $this->resource,
      array(
	'query' => array($this->idKey => $this->id),
      ));

    // Make the request.
    $this->response = drupal_http_request($this->url, array(
      'headers' => array('Authorization' => $this->apiKey),
    ));

    // Parse the response.
    $this->parseData();
  }

  /**
   * Parse the raw CKAN data into a standardised object.
   */
  private function parseData() {
    // Build a generic response object.
    $response_object = (object) array(
      'request_time' => time(),
      'code' => $this->response->code,
      'status' => $this->response->status,
      'data' => (object) array(),
      'valid' => $this->isValidResponse(),
    );

    // Data only gets populated if we have a valid response.
    if ($response_object->valid && isset($this->response->data->result)) {
      // TODO: Autodetect response format and handle errors if not JSON?
      $response_object->data = json_decode($this->response->data->result);
    }
    else {
      $this->errorLogger();
    }

    // Return our parsed response object.
    return $response_object;
  }

  /**
   * Check the response is OK and valid.
   */
  private function isValidResponse() {
    return (isset($this->response->code) && $this->response->code == 200);
  }

  /**
   * Handle errors.
   */
  private function errorLogger() {
    // Log to watchdog.
    $msg = t('Error requesting data from CKAN endpont: @url - Error @code - @status',
      array(
	'@url' => $this->url,
	'@code' => $this->response->code,
	'@status' => $this->response->status_message,
      )
    );
    watchdog('govcms_ckan_client', $msg, 'error');
  }

  /**
   * Caches remote data locally.
   */
  private function cacheData() {

  }

}

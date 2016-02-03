<?php
/**
 * @file
 * GovCmsCkanClient Class for fetching, caching and returning CKAN data.
 *
 * Basic Example Usage.
 * --------------------
 * $client = new GovCmsCkanClient();
 * $client->get('action/package_show', array('id' => 'fd49dc83f86f'));
 *
 * Response Object.
 * ----------------
 * A standardised format for the object returned. It is cached locally and
 * contains the following properties:
 * - valid: (bool) If the API request was a success or failure.
 * - request_time (int) The time of the request to the API.
 * - code: (int) Response code from the request to the API http request.
 * - status: (string) Status message from the API http request.
 * - data: (mixed) An object or array response from the API request.
 */

/**
 * Defines the GovCMS CKAN Client class.
 */
class GovCmsCkanClient {

  /**
   * API variables.
   */
  private $apiUrl;
  private $apiKey;
  private $apiPath = '/api/%d/';
  private $apiVersion = 3;

  /**
   * Request variables.
   */
  private $resource;
  private $query;
  private $url;

  /**
   * Response variable, gets updated whenever get/fetch is called.
   */
  private $response;

  /**
   * Response object, contains the object to be returned.
   */
  private $responseObject;

  /**
   * Drupal cache table.
   *
   * TODO: Define custom cache bin.
   * http://www.exove.com/techblog/drupal-7-cache-bins-why-and-how-to/
   */
  private $cacheTable = 'cache';

  /**
   * How long to cache for if request was successful (in seconds).
   */
  private $cacheExpirySuccess = 2592000;

  /**
   * How long to cache for if request failed (in seconds).
   */
  private $cacheExpiryFail = 86400;

  /**
   * Constructor.
   */
  public function __construct() {
    // Populate settings from Drupal variables.
    $this->getSettings();
  }

  /**
   * Test an endpoint is functional.
   *
   * @param string $endpoint
   *   The API endpont with no trailing slash.
   * @param string $resource
   *   The resource we are requesting.
   * @param array $query
   *   A key value pair of url paramaters.
   * @param string $api_key
   *   The api key for the endpoint (if required).
   * @param int $api_version
   *   The API version to test against.
   *
   * @return int
   *   The response code from the resource, 200 is success.
   */
  public function testConnection($endpoint, $resource, $query = array(), $api_key = NULL, $api_version = 3) {
    // Build a test url.
    $test_url = url(
      $endpoint . sprintf($this->apiPath, $api_version) . $resource,
      array(
	'external' => TRUE,
	'query' => $query,
      ));

    // Add authentication.
    $options = array();
    if (!is_null($api_key)) {
      $options = array('headers' => array('Authorization' => $api_key));
    }

    // Return TRUE if response code is 200 (OK).
    $response = drupal_http_request($test_url, $options);
    return $response->code;
  }

  /**
   * Return the data.
   *
   * @param string $resource
   *   Resource path.
   * @param array $query
   *   A key pair array forming the url paramaters.
   *
   * @return mixed
   *   Parsed response.
   */
  public function get($resource, $query = array()) {
    // Prepare the variables for the url.
    $this->resource = $resource;
    $this->query = $query;

    // Fetch the response.
    $this->fetch();

    // Return the response object.
    return $this->responseObject;
  }

  /**
   * Populate the settings using drupal variables.
   */
  private function getSettings() {
    $this->apiUrl = variable_get('govcms_ckan_endpoint_url', '');
    $this->apiKey = variable_get('govcms_ckan_api_key', '');
  }

  /**
   * Fetch CKAN data.
   */
  private function fetch() {
    $this->url = url(
      $this->apiUrl . sprintf($this->apiPath, $this->apiVersion) . $this->resource,
      array(
	'external' => TRUE,
	'query' => $this->query,
      ));

    // If no cache, we do a new request, parse and cache it.
    // If cache was present, the responseObject will be updated.
    if ($this->cacheDataGet() === FALSE) {
      // Make the request.
      $this->response = drupal_http_request($this->url, array(
	'headers' => array('Authorization' => $this->apiKey),
      ));

      // Parse the response.
      $this->parseData();

      // Save to cache.
      $this->cacheDataSet();
    }
  }

  /**
   * Parse the raw CKAN data into a standardised object.
   */
  private function parseData() {
    // Build a generic response object.
    $this->responseObject = (object) array(
      'valid' => $this->isValidResponse(),
      'request_time' => time(),
      'code' => $this->response->code,
      'status' => $this->response->status_message,
      'url' => $this->url,
      'data' => (object) array(),
    );

    // Data only gets populated if we have a valid response.
    if ($this->responseObject->valid && isset($this->response->data)) {
      // TODO: Autodetect response format and handle errors if not JSON?
      $data = json_decode($this->response->data);
      $this->responseObject->data = $data->result;
    }
    else {
      $this->errorLogger();
    }
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
    watchdog('govcms_ckan_client',
      'Error requesting data from CKAN endpont: @url - Error @code - @status',
      array(
	'@url' => $this->url,
	'@code' => $this->response->code,
	'@status' => $this->response->status_message,
      ),
      WATCHDOG_ERROR);
  }

  /**
   * Get cached request.
   *
   * @return mixed
   *   If a vaild cache hit found, it is returned, else FALSE on no cache found.
   */
  private function cacheDataGet() {
    $cache = cache_get($this->url);
    if (!empty($cache->data) && $cache->expire > time()) {
      $this->responseObject = $cache->data;
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Save current responseObject to Drupal cache.
   *
   * We use the url for the cache key as that will be unique to the request.
   */
  private function cacheDataSet() {
    $expiry = $this->responseObject->valid ? $this->cacheExpirySuccess : $this->cacheExpiryFail;
    cache_set($this->url, $this->responseObject, $this->cacheTable, time() + $expiry);
  }

}

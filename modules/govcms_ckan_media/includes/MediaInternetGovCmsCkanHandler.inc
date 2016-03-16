<?php
/**
 * @file
 * Extends the MediaInternetBaseHandler class to handle CKAN Datasets.
 */

/**
 * Implementation of MediaInternetBaseHandler.
 *
 * @see hook_media_internet_providers().
 */
class MediaInternetGovCmsCkanHandler extends MediaInternetBaseHandler {

  protected $baseUrl;
  protected $baseHost;
  protected $baseUri = 'ckan://resource/';

  /**
   * MediaInternetGovCmsCkanHandler constructor.
   */
  public function __construct($embed_code) {
    parent::__construct($embed_code);
    $this->baseUrl = variable_get('govcms_ckan_endpoint_url', '');
    $this->baseHost = $this->getBaseHost();
  }

  /**
   * Parse a dataset url to return a normalised URI with the resource ID.
   *
   * @return string
   *   A normalised file URI. FALSE if no match.
   */
  public function parse($embed_code) {

    // @baseurl/dataset/PACKAGE_ID/resource/*
    // @baseurl/api/3/action/resource_show?id=*
    $patterns = array(
      '@' . $this->baseHost . '/dataset/(?:[^"\&\? ]+)/resource/([^"\&\? ]+)@i',
      '@' . $this->baseHost . '/api/3/action/resource_show[#\?].*?id=([^"\&\? ]+)@i',
    );

    // First check if the url contains the embed code.
    if ($this->isCkanUrl($embed_code)) {
      foreach ($patterns as $pattern) {
        preg_match($pattern, $embed_code, $matches);
        if (isset($matches[1])) {
          return file_stream_wrapper_uri_normalize($this->baseUri . $matches[1]);
        }
      }
    }

    return FALSE;
  }

  /**
   * Determines if this handler should claim the item.
   *
   * @param string $embed_code
   *   A string of user-submitted embed code.
   *
   * @return bool
   *   Pass TRUE to claim the item.
   */
  public function claim($embed_code) {
    return $this->parse($embed_code) !== FALSE;
  }

  /**
   * Returns a file object which can be used for validation.
   *
   * @return StdClass
   *   A file object.
   */
  public function getFileObject() {
    $uri = $this->parse($this->embedCode);
    $file = file_uri_to_object($uri, TRUE);

    // Try to default the file name to the resource's title.
    if (empty($file->fid) && $info = $this->getResource($uri)) {
      $file->filename = truncate_utf8($info->name, 255);
    }

    return $file;
  }

  /**
   * Returns information about the resource.
   *
   * @param string $uri
   *   The Drupal file URI for this resource.
   *
   * @return object
   *   If resource information is available, an object containing 'name', 'id',
   *   'url', and other information as specified by the CKAN standard.
   *   Otherwise, NULL.
   *
   * @throws Exception
   *   Error if unable to load the resource.
   */
  public function getResource($uri) {
    $client = govcms_ckan_client();
    $meta = $client->get('action/resource_show', array('id' => $this->getResourceIdFromUri($uri)));

    if ($meta->valid === TRUE) {
      return $meta->data;
    }
    else {
      throw new Exception("Unable to retrieve resource information. (Error: {$meta->code}, {$meta->status})");
    }
  }

  /**
   * Extract the CKAN resource ID from a Drupal URI.
   *
   * @param string $uri
   *   The drupal file uri.
   *
   * @return string
   *   The resource ID.
   */
  public function getResourceIdFromUri($uri) {
    return str_replace($this->baseUri, '', $uri);
  }

  /**
   * This simply checks if the domain from config exists in the url string.
   *
   * @param string $url
   *   The url string entered by the user.
   *
   * @return bool
   *   TRUE if match, FALSE if not.
   */
  public function isCkanUrl($url) {
    return (!empty($url) && !empty($this->baseHost) && strpos($url, $this->baseHost) !== FALSE);
  }

  /**
   * Get the base host.
   *
   * @return mixed
   *   If valid host found it is returned, otherwise FALSE.
   */
  public function getBaseHost() {
    $url_parts = parse_url($this->baseUrl);
    return (!empty($url_parts['host']) ? $url_parts['host'] : NULL);
  }

}

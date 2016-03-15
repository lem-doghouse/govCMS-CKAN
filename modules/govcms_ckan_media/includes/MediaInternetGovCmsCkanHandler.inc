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

  /**
   * MediaInternetGovCmsCkanHandler constructor.
   */
  public function __construct($embed_code) {
    parent::__construct($embed_code);
    $this->baseUrl = variable_get('govcms_ckan_endpoint_url', '');
  }

  /**
   * Parse a dataset url to return the endpoint.
   *
   * @return string
   *   A normalised file URI.
   */
  public function parse($embed_code) {
    // TODO: Add parsing that supports pulling out the ID from a url.
    // For now just assuming the $embedCode is a package ID.
    return file_stream_wrapper_uri_normalize('ckan://package/' . $embed_code);
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
    //return (!empty($this->baseUrl) && strpos($embedCode, $this->baseUrl) !== FALSE);
    // TODO: Test if this should be claimed.
    return TRUE;
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

    // Try to default the file name to the video's title.
    if (empty($file->fid) && $info = $this->getPackage()) {
      $file->filename = truncate_utf8($info->name, 255);
    }

    return $file;
  }

  /**
   * Returns information about the package.
   *
   * See http://www.oembed.com.
   *
   * @return object
   *   If package information is available, an object containing 'name', 'id',
   *   'url', and other information as specified by the CKAN standard.
   *   Otherwise, NULL.
   *
   * @throws Exception
   *   Error if unable to load the resource.
   */
  public function getPackage() {
    $client = govcms_ckan_client();
    $meta = $client->get('action/resource_show', array('id' => $this->embedCode));

    if ($meta->valid === TRUE) {
      return $meta->data;
    }
    else {
      throw new Exception("Error Processing Request. (Error: {$meta->code}, {$meta->status})");
    }
  }

}

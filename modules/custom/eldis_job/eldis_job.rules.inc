<?php

/**
 * @file
 * Eldis jobs rules definitions.
 */

/**
 * Implements hook_rules_event_info().
 *
 * Define some events that can be used to trigger actions
 * in the Rules UI.
 */
function eldis_job_rules_event_info() {
  $events = array();

  $events['eldis_license_sponsored_new_suspended'] = array(
    'label' => t('After suspending a newly created sponsored license'),
    'group' => t('Eldis jobs'),
    'variables' => array(
      'commerce_license' => array(
        'type' => 'commerce_license',
        'label' => t('The license'),
      ),
    ),
  );

  return $events;
}

/**
 * Implements hook_rules_action_info().
 */
function eldis_job_rules_action_info() {
  $actions = array();
  $actions['eldis_job_license_suspend_sponsored'] = array(
    'label' => t('Set the state of a new "Sponsored" license to "Suspended"'),
    'parameter' => array(
      'commerce_license' => array(
        'type' => 'commerce_license',
        'label' => t('Newly created "sponsored" license'),
      ),
    ),
    'group' => t('Eldis jobs'),
    'callbacks' => array(
      'execute' => 'eldis_job_license_suspend_sponsored',
    ),
  );

  $actions['eldis_job_license_revoke_pay_per_job'] = array(
    'label' => t('Set the state of a "Pay per job" license to "Revoked"'),
    'parameter' => array(
      'node' => array(
        'type' => 'job',
        'label' => t('Newly created job'),
      ),
    ),
    'group' => t('Eldis jobs'),
    'callbacks' => array(
      'execute' => 'eldis_job_license_revoke_pay_per_job',
    ),
  );

  return $actions;
}

/**
 * Rules action callback: Suspend a Sponsored license
 *
 * Changes the status of the license that gets created for a Sponsored subscription
 * to COMMERCE_LICENSE_SUSPENDED in order that the "Sponsored" application can be
 * manually reviewed and then either "revoked" or "activated".
 *
 *
 */
function eldis_job_license_suspend_sponsored($commerce_license) {

  // get type of referenced product, filter on product type
  $commerce_license_wrapper = entity_metadata_wrapper('commerce_license', $commerce_license);
  $product = $commerce_license_wrapper->product;
  $sku = $product->sku->value();

  /**
   *
  define('ELDIS_JOB_PLAN_SPONSORED_PRODUCT_SKU', 'job_plan_sponsored');
  define('ELDIS_JOB_PLAN_PREMIUM_SUBSCRIPTION_PRODUCT_SKU', 'job-plan-premium-subscription');
  define('ELDIS_JOB_PLAN_PREMIUM_PAY_PER_POST_PRODUCT_SKU', 'job-plan-premium-pay-per-post');
   */

  // ensure we only do this for the Sponsored type
  if($sku == ELDIS_JOB_PLAN_SPONSORED_PRODUCT_SKU) {
    // update the status
    $commerce_license_wrapper->set();

    // fire "initial" contact event to inform applicant
    // "eldis_license_sponsored_new_suspended"
  }





  $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
  $billing_start = $line_item_wrapper->cl_billing_start->value();
  $billing_end = $line_item_wrapper->cl_billing_end->value();
  $duration = $billing_end - $billing_start;
  $billing_cycle = $line_item_wrapper->order->cl_billing_cycle->value();
  commerce_license_billing_prorate_line_item($line_item_wrapper, $duration, $billing_cycle);
}

/**
 * Rules action callback: Revoke context user's pay per job license.
 *
 * - get user from job
 * - get latest order from user
 * - get latest license from order
 * - update license
 *
 * - direct query to get latest license for this user
 * - check that it is of the required type
 * - load the license
 * -
 */
function eldis_job_license_revoke_pay_per_job($job) {

  // get the user from the job
  $user = user_load($job->uid);

  // Load the user latest order
  $query = new EntityFieldQuery();
  $query
    ->entityCondition('entity_type', 'commerce_order', '=')
    ->propertyCondition('uid', $account->uid, '=')
    ->propertyCondition('status', array_keys(commerce_order_statuses(array('cart' => TRUE))), 'IN');

  $result = $query->execute();

  print_r($result);
  exit;

  if (!empty($result['commerce_order'])) {
    foreach (commerce_order_load_multiple(array_keys($result['commerce_order'])) as $order) {
      if ($order->mail != $account->mail) {
        $order->mail = $account->mail;
        commerce_order_save($order);
      }
    }
  }





  // check for a users active license, use EMW
  // .. load commerce license
  $licenses = commerce_license_get_order_licenses($result);
  $license = array_unshift($licenses); // check that this would give

  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  foreach ($wrapper->commerce_line_items as $line_item_wrapper) {
    if (isset($line_item_wrapper->commerce_license)) {
      $license = $line_item_wrapper->commerce_license->value();
      if ($license && (!$configurable || $license->isConfigurable())) {
        $licenses[$license->license_id] = $license;
      }
    }
  }

  if($license)
  // if license is pay per job
  // load product and check sku (job-plan-premium-pay-per-post is the sku)

  // set license status to revoke
  commerce_license_revoke_action($license);

  // @todo consider events to be fired here?


  $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
  $billing_start = $line_item_wrapper->cl_billing_start->value();
  $billing_end = $line_item_wrapper->cl_billing_end->value();
  $duration = $billing_end - $billing_start;
  $billing_cycle = $line_item_wrapper->order->cl_billing_cycle->value();
  commerce_license_billing_prorate_line_item($line_item_wrapper, $duration, $billing_cycle);
}


/**
 * Rules action callback helper
 *
 * Check how many jobs have been created by this user
 */
function eldis_jobs_count_users_jobs($user) {
  // entity field query to get how many jobs user has


  return $count;
}
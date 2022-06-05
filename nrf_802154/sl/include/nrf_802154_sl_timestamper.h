/*
 * Copyright (c) 2022, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * @brief Module that provides the timestamper service for the 802.15.4 driver.
 *
 * The @b timestamper allows timestamping of hardware events occuring in peripherals with
 * microsecond time resolution. Object of type @ref nrf_802154_sl_timestamper_t
 * (and related functions) is responsible for this feature.
 */

#ifndef NRF_802154_SL_TIMESTAMPER_H_
#define NRF_802154_SL_TIMESTAMPER_H_

#include <stdint.h>
#include <stdbool.h>
#include "nrf_802154_sl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_802154_sl_timestamper Timer Service
 * @{
 * @ingroup nrf_802154_sl_timestamper
 * @brief The Timer Service for the 802.15.4 driver.
 *
 */

/**@brief Number of array elements required by @ref nrf_802154_sl_timestamper_priv_placeholder_t. */
#define NRF_802154_SL_TIMESTAMPER_PRIV_SIZE 1U

/**@brief Type representing a timestamper object. */
typedef struct nrf_802154_sl_timestamper_s nrf_802154_sl_timestamper_t;

/**@brief Structure that represents private fields of a timestamper required
 *        by the implementation. */
__ALIGN(8) typedef struct
{
    uint64_t placeholder[NRF_802154_SL_TIMESTAMPER_PRIV_SIZE];
} nrf_802154_sl_timestamper_priv_placeholder_t;

struct nrf_802154_sl_timestamper_s
{
    /**@brief Private fields required by the implementation. */
    nrf_802154_sl_timestamper_priv_placeholder_t priv;
};

/**@brief Type of result returned by timer-related functions.
 *
 * Possible values:
 * @ref NRF_802154_SL_TIMESTAMPER_RET_SUCCESS,
 * @ref NRF_802154_SL_TIMESTAMPER_RET_NO_RESOURCES,
 * @ref NRF_802154_SL_TIMESTAMPER_RET_BAD_STATE,
 * @ref NRF_802154_SL_TIMESTAMPER_RET_NO_EVENT,
 * @ref NRF_802154_SL_TIMESTAMPER_RET_TRY_LATER.
 */
typedef uint32_t nrf_802154_sl_timestamper_ret_t;

/**@brief Operation on a timestamper succeed. */
#define NRF_802154_SL_TIMESTAMPER_RET_SUCCESS      0U

/**@brief Operation on a timestamper failed due to lack of required resources.
 *
 * Timestamper may require hardware compare channels of fine resolution timer
 * to bind given event to timer's capture task. This result indicates that these
 * resources were unavailable.
 */
#define NRF_802154_SL_TIMESTAMPER_RET_NO_RESOURCES 1U

/**@brief The state of a timestamper object was inappropriate for requested operation. */
#define NRF_802154_SL_TIMESTAMPER_RET_BAD_STATE    2U

/**@brief Event which should be timestamped has not triggered yet. */
#define NRF_802154_SL_TIMESTAMPER_RET_NO_EVENT     3U

/**@brief Calculation of timestamp is impossible due to not synchronized fine and coarse
 *        resolution timers.
 *
 * Please refer to @ref nrf_802154_sl_timestamper_timestamp_get for details.
 */
#define NRF_802154_SL_TIMESTAMPER_RET_TRY_LATER    4U

/**@brief Initializes an instance of a timestamper object.
 *
 * This function plays a role of a constructor. It should be called once
 * per timestamper instance before any other API call related to given
 * timestamper instance. Calling any other API related to a timestamper
 * instance before call to @ref nrf_802154_sl_timestamper_init or
 * calling @ref nrf_802154_sl_timestamper_init more than once is an
 * undefined behavior.
 *
 * @param p_timestamper  Pointer to a timestamper object instance to be initialized.
 */
void nrf_802154_sl_timestamper_init(nrf_802154_sl_timestamper_t * p_timestamper);

/**@brief De-initializes an instance of a timestamper object.
 *
 * This function frees the resources associated with the timestamper object.
 *
 * @param p_timestamper  Pointer to a timestamper object instance to be de-initialized.
 */
void nrf_802154_sl_timestamper_deinit(nrf_802154_sl_timestamper_t * p_timestamper);

/**@brief Prepares timestamper instance to allow timestamping of a hardware event.
 *
 * Timestamper can make a single timestamp of a hardware event.
 * In case of successful execution this functions causes:
 * - creation of hardware bindings between given (D)PPI channel and fine resolution timer
 * - start of fine resolution timer if not running yet.
 *
 * When the hardware event connected to the provided (D)PPI channel occurs, timestamp is triggered.
 * The API user is responsible for creating all necessary bindings in hardware between the event
 * to be timestamped and the provided (D)PPI channel, as well as any necessary configuration of
 * these resources. Moreover, the API user must keep the hardware bindings enabled and functional
 * until the event is propagated through the provided (D)PPI channel. If the API user fails
 * to do that, the timestamp will not be taken. That condition can be satisfied easily if the user
 * keeps the hardware bindings enabled and functional until @ref nrf_802154_sl_timestamper_get is called.
 *
 * The timer module does not provide any way of triggering software action in response to an event.
 * Providing such feature, i.e. appropriate ISR for given event, is a responsibilty
 * of a user. The user should call @ref nrf_802154_sl_timestamper_timestamp_get as early after
 * event occurrence as practically possible to release hardware resources used for timestamping.
 * This way these resources can be re-used later.
 *
 * When the user is done with the timestamper @ref nrf_802154_sl_timestamper_cleanup
 * should be called. When the user wishes to re-use the timestamper instance,
 * @ref @ref nrf_802154_sl_timestamper_cleanup should be called first and then
 * @ref nrf_802154_sl_timestamper_setup should be called.
 *
 * @param p_timestamper Pointer to a timestamper object that is responsible for
 *                      keeping timestamper state.
 * @param ppi_channel   Identifier of (D)PPI channel that the hardware event to be timestamped
 *                      by @p p_timestamper publishes to. If @p ppi_channel represents
 *                      PPI channel (as opposed to DPPI channel), then the TEP register associated
 *                      with that channel must not be configured (be equal to 0),
 *                      as the timestamping mechanism relies on it to be free.
 *
 * @retval NRF_802154_SL_TIMESTAMPER_RET_SUCCESS
 *      The timestamper setup was successful.
 *
 * @retval NRF_802154_SL_TIMESTAMPER_RET_NO_RESOURCES
 *      There was no hardware resources to perform timestamping.
 *
 * @retval NRF_802154_SL_TIMESTAMPER_RET_BAD_STATE
 *      The timestamper was already setup to perform timestamping and no cleanup
 *      has been called yet.
 */
nrf_802154_sl_timestamper_ret_t nrf_802154_sl_timestamper_setup(
    nrf_802154_sl_timestamper_t        * p_timestamper,
    const nrf_802154_sl_event_handle_t * p_event);

/**@brief Gets timestamp made by a timestamper.
 *
 * This function fetches data allowing calculation of absolute timestamp and
 * calculates the timestamp if all data necessary is available.
 *
 * When the function succeeds at fetching the timestamp, some resources required for
 * timestamping (compare channels, hardware event bindings) are released and can be
 * re-used.
 *
 * Call to this function does not stop fine resolution timer as this can be done by
 * @ref nrf_802154_sl_timestamper_cleanup only.
 *
 * The timestamper object can be re-used by call to @ref nrf_802154_sl_timestamper_cleanup
 * and then @ref nrf_802154_sl_timestamper_setup.
 *
 * @param p_timestamper     Pointer to a timestamper instance.
 * @param p_timestamp       Timestamp in microseconds of an event will be written
 *                          at this pointer if the function returns
 *                          @ref NRF_802154_SL_TIMESTAMPER_RET_SUCCESS.
 *
 * @retval NRF_802154_SL_TIMESTAMPER_RET_SUCCESS
 *      The timestamp is available and written at p_timestamp pointer.
 *      The hardware bindings between event and timer have been cleaned
 *      up as they have become not necessary.
 *
 * @retval NRF_802154_SL_TIMESTAMPER_RET_BAD_STATE
 *      The timestamper has not been setup for timestamping, i.e.
 *      @ref nrf_802154_sl_timestamper_setup has not been called before.
 *
 * @retval NRF_802154_SL_TIMESTAMPER_RET_NO_EVENT
 *      The event being timestamped has not triggered yet.
 *
 * @retval NRF_802154_SL_TIMESTAMPER_RET_TRY_LATER
 *      The event has triggered and the data allowing timestamping is fetched, but
 *      currently this data doesn't allow calculation of absolute timestamp.
 *      The timestamp will be available later when coarse and fine clocks are synchronized.
 *      The hardware bindings between event and timer have been cleaned
 *      up as they have become not necessary.
 */
nrf_802154_sl_timestamper_ret_t nrf_802154_sl_timestamper_timestamp_get(
    nrf_802154_sl_timestamper_t * p_timestamper,
    uint64_t                    * p_timestamp);

/**@brief Cleans up after timestamping.
 *
 * This function definitely ends timestamping process related to given timestamper instance.
 * This function must be called complementary to @ref nrf_802154_sl_timestamper_setup
 * no matter if user managed to get the timestamp or not.
 *
 * This function is responsible for cleaning up after @ref nrf_802154_sl_timestamper_setup.
 * It removes any hardware bindings (if still existing) made
 * by @ref nrf_802154_sl_timestamper_setup, frees any resources allocated by the call and
 * releases fine resolution timer (if still requested). Please note that fine resolution timer
 * can be left running if other resources require it (other timestampers, timers etc.).
 *
 * When the user calls this function on a timestamper instance which has not been setup
 * for timestamping, i.e. @ref nrf_802154_sl_timestamper_setup has not been called,
 * the call does nothing.
 *
 * If user didn't call @ref nrf_802154_sl_timestamper_timestamp_get the timestamp
 * (if it has been ever made) will be lost, but it allows abandoning of timestamping
 * and releases used resources.
 *
 * @param p_timestamper  Pointer to a timestamper instance to be cleaned up.
 */
void nrf_802154_sl_timestamper_cleanup(nrf_802154_sl_timestamper_t * p_timestamper);

/**@brief Initializes the timestamper module.
 *
 * This function initializes the global timestamper module state, if any exists.
 */
void nrf_802154_sl_timestamper_module_init(void);

/**@brief Deinitialize the timestamper module.
 *
 * This function deinitializes the global timestamper module state, if any exists.
 */
void nrf_802154_sl_timestamper_module_deinit(void);

/**
 *@}
 **/

#ifdef __cplusplus
}
#endif

#endif /* NRF_802154_SL_TIMESTAMPER_H_ */

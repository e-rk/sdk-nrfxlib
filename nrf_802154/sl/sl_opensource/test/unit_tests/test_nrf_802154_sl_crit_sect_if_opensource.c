/*
 * Copyright (c) 2025, Nordic Semiconductor ASA
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

#include "unity.h"

#include <stdbool.h>
#include <stdint.h>

#include "nrf_802154_sl_crit_sect_if.h"

/**
 * @brief Mock critical section interface for testing.
 */
static nrf_802154_sl_crit_sect_interface_t m_test_interface;

/**
 * @brief Mock critical section enter function.
 */
static bool m_mock_enter_called;
static bool m_mock_enter_return_value;

/**
 * @brief Mock critical section exit function.
 */
static bool m_mock_exit_called;

/**
 * @brief Mock function for entering critical section.
 *
 * @return bool Mock return value set by test.
 */
static bool mock_crit_sect_enter(void)
{
    m_mock_enter_called = true;
    return m_mock_enter_return_value;
}

/**
 * @brief Mock function for exiting critical section.
 */
static void mock_crit_sect_exit(void)
{
    m_mock_exit_called = true;
}

/**
 * @brief Reset mock state before each test.
 */
void setUp(void)
{
    m_mock_enter_called       = false;
    m_mock_exit_called        = false;
    m_mock_enter_return_value = true;

    // Reset the global interface pointer
    extern const nrf_802154_sl_crit_sect_interface_t * gp_nrf_802154_sl_crit_sect_interface;

    gp_nrf_802154_sl_crit_sect_interface = NULL;

    // Setup test interface
    m_test_interface.enter = mock_crit_sect_enter;
    m_test_interface.exit  = mock_crit_sect_exit;
}

/**
 * @brief Cleanup after each test.
 */
void tearDown(void)
{
    // No cleanup needed
}

/**
 * @brief Test that nrf_802154_sl_crit_sect_enter calls the interface function and returns correct value.
 */
void test_nrf_802154_sl_crit_sect_enter_calls_interface_function_returns_true(void)
{
    // Initialize with test interface
    nrf_802154_sl_crit_sect_init(&m_test_interface);

    // Set mock to return true
    m_mock_enter_return_value = true;

    // Call the inline function
    bool result = nrf_802154_sl_crit_sect_enter();

    // Verify the mock function was called and return value is correct
    TEST_ASSERT_TRUE(m_mock_enter_called);
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Test that nrf_802154_sl_crit_sect_enter returns false when interface function returns false.
 */
void test_nrf_802154_sl_crit_sect_enter_calls_interface_function_returns_false(void)
{
    // Initialize with test interface
    nrf_802154_sl_crit_sect_init(&m_test_interface);

    // Set mock to return false
    m_mock_enter_return_value = false;

    // Call the inline function
    bool result = nrf_802154_sl_crit_sect_enter();

    // Verify the mock function was called and return value is correct
    TEST_ASSERT_TRUE(m_mock_enter_called);
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Test that nrf_802154_sl_crit_sect_exit calls the interface function.
 */
void test_nrf_802154_sl_crit_sect_exit_calls_interface_function(void)
{
    // Initialize with test interface
    nrf_802154_sl_crit_sect_init(&m_test_interface);

    // Call the inline function
    nrf_802154_sl_crit_sect_exit();

    // Verify the mock function was called
    TEST_ASSERT_TRUE(m_mock_exit_called);
}

/**
 * @brief Test critical section enter/exit sequence.
 */
void test_nrf_802154_sl_crit_sect_enter_exit_sequence(void)
{
    // Initialize with test interface
    nrf_802154_sl_crit_sect_init(&m_test_interface);

    // Verify initial state
    TEST_ASSERT_FALSE(m_mock_enter_called);
    TEST_ASSERT_FALSE(m_mock_exit_called);

    // Enter critical section
    bool enter_result = nrf_802154_sl_crit_sect_enter();

    TEST_ASSERT_TRUE(m_mock_enter_called);
    TEST_ASSERT_FALSE(m_mock_exit_called);
    TEST_ASSERT_TRUE(enter_result);

    // Exit critical section
    nrf_802154_sl_crit_sect_exit();
    TEST_ASSERT_TRUE(m_mock_enter_called);
    TEST_ASSERT_TRUE(m_mock_exit_called);
}

/**
 * @brief Test multiple enter/exit calls.
 */
void test_nrf_802154_sl_crit_sect_multiple_enter_exit_calls(void)
{
    // Initialize with test interface
    nrf_802154_sl_crit_sect_init(&m_test_interface);

    // First enter/exit sequence
    bool result1 = nrf_802154_sl_crit_sect_enter();

    TEST_ASSERT_TRUE(result1);
    nrf_802154_sl_crit_sect_exit();

    // Reset mock state
    m_mock_enter_called = false;
    m_mock_exit_called  = false;

    // Second enter/exit sequence
    bool result2 = nrf_802154_sl_crit_sect_enter();

    TEST_ASSERT_TRUE(result2);
    TEST_ASSERT_TRUE(m_mock_enter_called);

    nrf_802154_sl_crit_sect_exit();
    TEST_ASSERT_TRUE(m_mock_exit_called);
}

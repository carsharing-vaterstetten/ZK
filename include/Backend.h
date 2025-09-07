#pragma once

// Server router paths

#define LOG_ROUTE "/log"
#define FIRMWARE_ROUTE "/firmware"
#define RFIDS_ROUTE "/rfids"
#define STATIC_ROUTE "/static"

// Log route
#define LOG_FILE_UPLOAD_ENDPOINT LOG_ROUTE "/upload"

// Firmware route
#define LATEST_FIRMWARE_ENDPOINT FIRMWARE_ROUTE "/latest"
#define LATEST_FIRMWARE_DOWNLOAD_PATH LATEST_FIRMWARE_ENDPOINT
#define LATEST_FIRMWARE_IS_NEWER_ENDPOINT FIRMWARE_ROUTE "/is-newer-available"
#define LATEST_FIRMWARE_SIZE_ENDPOINT LATEST_FIRMWARE_ENDPOINT "/latest"

// RFIDs route
#define REMOTE_RFID_MD5_CHECKSUM_PATH RFIDS_ROUTE "/md5-checksum"
#define REMOTE_RFID_PATH RFIDS_ROUTE "/download"

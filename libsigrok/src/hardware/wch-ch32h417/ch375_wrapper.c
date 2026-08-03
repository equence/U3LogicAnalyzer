/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on libsigrok.
 *
 * Copyright (C) 2026 Q2H2
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * CH375DLL动态加载封装实现
 * 使用LoadLibrary动态加载DLL函数
 */

#include <config.h>
#include "ch375_wrapper.h"
#include <libsigrok/libsigrok.h>
#include "libsigrok-internal.h"
#include <glib.h>

/* Windows 平台使用 CH375DLL（原实现）；其他平台使用 libusb 传输层（见文件尾部） */
#ifdef _WIN32
#include <windows.h>
#endif

#define LOG_PREFIX "ch375"

#ifdef _WIN32

/* ============================================================================
 * DLL句柄和函数指针
 * ============================================================================ */

static HMODULE ch375_dll_handle = NULL;

/* 设备管理函数指针 */
static CH375OpenDevice_func pCH375OpenDevice = NULL;
static CH375CloseDevice_func pCH375CloseDevice = NULL;
static CH375GetVersion_func pCH375GetVersion = NULL;
static CH375GetDrvVersion_func pCH375GetDrvVersion = NULL;
static CH375GetUsbID_func pCH375GetUsbID = NULL;
static CH375GetDeviceName_func pCH375GetDeviceName = NULL;

/* 设备描述符函数指针 */
static CH375GetDeviceDescr_func pCH375GetDeviceDescr = NULL;
static CH375GetConfigDescr_func pCH375GetConfigDescr = NULL;

/* 设备控制函数指针 */
static CH375ResetDevice_func pCH375ResetDevice = NULL;
static CH375SetExclusive_func pCH375SetExclusive = NULL;

/* 超时设置函数指针 */
static CH375SetTimeout_func pCH375SetTimeout = NULL;
static CH375SetTimeoutEx_func pCH375SetTimeoutEx = NULL;

/* 端点1直接上下传函数指针 */
static CH375WriteRead_func pCH375WriteRead = NULL;

/* 端点读写函数指针 */
static CH375WriteEndP_func pCH375WriteEndP = NULL;
static CH375ReadEndP_func pCH375ReadEndP = NULL;

/* 端点控制函数指针 */
static CH375AbortEndPRead_func pCH375AbortEndPRead = NULL;
static CH375AbortEndPWrite_func pCH375AbortEndPWrite = NULL;
static CH375ResetInEndP_func pCH375ResetInEndP = NULL;
static CH375ResetOutEndP_func pCH375ResetOutEndP = NULL;

/* 数据端点读写函数指针 */
static CH375ReadData_func pCH375ReadData = NULL;
static CH375WriteData_func pCH375WriteData = NULL;
static CH375AbortRead_func pCH375AbortRead = NULL;
static CH375AbortWrite_func pCH375AbortWrite = NULL;

/* 缓冲上传模式函数指针 */
static CH375SetBufUpload_func pCH375SetBufUpload = NULL;
static CH375QueryBufUpload_func pCH375QueryBufUpload = NULL;
static CH375SetBufUploadEx_func pCH375SetBufUploadEx = NULL;
static CH375QueryBufUploadEx_func pCH375QueryBufUploadEx = NULL;
static CH375ClearBufUpload_func pCH375ClearBufUpload = NULL;

/* 缓冲下传模式函数指针 */
static CH375SetBufDownload_func pCH375SetBufDownload = NULL;
static CH375QueryBufDownload_func pCH375QueryBufDownload = NULL;
static CH375SetBufDownloadEx_func pCH375SetBufDownloadEx = NULL;

/* 驱动命令函数指针 */
static CH375DriverCommand_func pCH375DriverCommand = NULL;

/* 设备事件通知函数指针 */
static CH375SetDeviceNotify_func pCH375SetDeviceNotify = NULL;

/* ============================================================================
 * 动态加载辅助宏
 * ============================================================================ */

#define LOAD_FUNC(name) \
    do { \
        p##name = (name##_func)GetProcAddress(ch375_dll_handle, #name); \
        if (!p##name) { \
            sr_err("Failed to get " #name " function address"); \
            return SR_ERR; \
        } \
    } while(0)

#define LOAD_FUNC_OPTIONAL(name) \
    do { \
        p##name = (name##_func)GetProcAddress(ch375_dll_handle, #name); \
    } while(0)

/* ============================================================================
 * DLL加载/卸载
 * ============================================================================ */

int ch375_init(void)
{
    if (ch375_dll_handle != NULL) {
        /* 已经加载 */
        return SR_OK;
    }

    /* 尝试加载CH375DLL64.DLL */
    ch375_dll_handle = LoadLibraryA("CH375DLL64.DLL");
    if (ch375_dll_handle == NULL) {
        /* 尝试加载CH375DLL.DLL (32位版本) */
        ch375_dll_handle = LoadLibraryA("CH375DLL.DLL");
        if (ch375_dll_handle == NULL) {
            sr_err("Failed to load CH375DLL64.DLL or CH375DLL.DLL");
            return SR_ERR;
        }
        sr_warn("Loaded CH375DLL.DLL (32-bit), consider using 64-bit version");
    }

    /* 加载所有必需的函数 */
    LOAD_FUNC(CH375OpenDevice);
    LOAD_FUNC(CH375CloseDevice);
    LOAD_FUNC(CH375GetUsbID);
    LOAD_FUNC(CH375SetTimeout);
    LOAD_FUNC(CH375WriteRead);
    LOAD_FUNC(CH375ReadEndP);
    LOAD_FUNC(CH375SetBufUploadEx);
    LOAD_FUNC(CH375QueryBufUploadEx);
    LOAD_FUNC(CH375ClearBufUpload);
    LOAD_FUNC(CH375AbortEndPRead);

    /* 加载可选函数 */
    LOAD_FUNC_OPTIONAL(CH375GetVersion);
    LOAD_FUNC_OPTIONAL(CH375GetDrvVersion);
    LOAD_FUNC_OPTIONAL(CH375GetDeviceName);
    LOAD_FUNC_OPTIONAL(CH375GetDeviceDescr);
    LOAD_FUNC_OPTIONAL(CH375GetConfigDescr);
    LOAD_FUNC_OPTIONAL(CH375ResetDevice);
    LOAD_FUNC_OPTIONAL(CH375SetExclusive);
    LOAD_FUNC_OPTIONAL(CH375SetTimeoutEx);
    LOAD_FUNC_OPTIONAL(CH375WriteEndP);
    LOAD_FUNC_OPTIONAL(CH375AbortEndPWrite);
    LOAD_FUNC_OPTIONAL(CH375ResetInEndP);
    LOAD_FUNC_OPTIONAL(CH375ResetOutEndP);
    LOAD_FUNC_OPTIONAL(CH375ReadData);
    LOAD_FUNC_OPTIONAL(CH375WriteData);
    LOAD_FUNC_OPTIONAL(CH375AbortRead);
    LOAD_FUNC_OPTIONAL(CH375AbortWrite);
    LOAD_FUNC_OPTIONAL(CH375SetBufUpload);
    LOAD_FUNC_OPTIONAL(CH375QueryBufUpload);
    LOAD_FUNC_OPTIONAL(CH375SetBufDownload);
    LOAD_FUNC_OPTIONAL(CH375QueryBufDownload);
    LOAD_FUNC_OPTIONAL(CH375SetBufDownloadEx);
    LOAD_FUNC_OPTIONAL(CH375DriverCommand);
    LOAD_FUNC_OPTIONAL(CH375WriteData);
    LOAD_FUNC_OPTIONAL(CH375SetDeviceNotify);

    sr_info("CH375DLL loaded successfully");
    return SR_OK;
}

void ch375_cleanup(void)
{
    if (ch375_dll_handle != NULL) {
        FreeLibrary(ch375_dll_handle);
        ch375_dll_handle = NULL;

        /* 清空所有函数指针 */
        pCH375OpenDevice = NULL;
        pCH375CloseDevice = NULL;
        pCH375GetVersion = NULL;
        pCH375GetDrvVersion = NULL;
        pCH375GetUsbID = NULL;
        pCH375GetDeviceName = NULL;
        pCH375GetDeviceDescr = NULL;
        pCH375GetConfigDescr = NULL;
        pCH375ResetDevice = NULL;
        pCH375SetExclusive = NULL;
        pCH375SetTimeout = NULL;
        pCH375SetTimeoutEx = NULL;
        pCH375WriteRead = NULL;
        pCH375WriteEndP = NULL;
        pCH375ReadEndP = NULL;
        pCH375AbortEndPRead = NULL;
        pCH375AbortEndPWrite = NULL;
        pCH375ResetInEndP = NULL;
        pCH375ResetOutEndP = NULL;
        pCH375ReadData = NULL;
        pCH375WriteData = NULL;
        pCH375AbortRead = NULL;
        pCH375AbortWrite = NULL;
        pCH375SetBufUpload = NULL;
        pCH375QueryBufUpload = NULL;
        pCH375SetBufUploadEx = NULL;
        pCH375QueryBufUploadEx = NULL;
        pCH375ClearBufUpload = NULL;
        pCH375SetBufDownload = NULL;
        pCH375QueryBufDownload = NULL;
        pCH375SetBufDownloadEx = NULL;
        pCH375DriverCommand = NULL;
        pCH375SetDeviceNotify = NULL;

        sr_info("CH375DLL unloaded");
    }
}

int ch375_is_loaded(void)
{
    return (ch375_dll_handle != NULL);
}

/* ============================================================================
 * 设备管理
 * ============================================================================ */

void* ch375_open_device(unsigned long index)
{
    if (!pCH375OpenDevice) {
        sr_err("CH375OpenDevice not loaded");
        return CH375_INVALID_HANDLE;
    }

    void* handle = pCH375OpenDevice(index);
    if (handle == CH375_INVALID_HANDLE || handle == NULL) {
        sr_dbg("Failed to open device at index %lu", index);
        return CH375_INVALID_HANDLE;
    }

    sr_dbg("Opened device at index %lu", index);
    return handle;
}

void ch375_close_device(unsigned long index)
{
    if (pCH375CloseDevice) {
        pCH375CloseDevice(index);
        sr_dbg("Closed device at index %lu", index);
    }
}

unsigned long ch375_get_usb_id(unsigned long index)
{
    if (!pCH375GetUsbID) {
        sr_err("CH375GetUsbID not loaded");
        return 0;
    }
    return pCH375GetUsbID(index);
}

const char* ch375_get_device_name(unsigned long index)
{
    if (!pCH375GetDeviceName) {
        return NULL;
    }
    return (const char*)pCH375GetDeviceName(index);
}

int ch375_get_device_descr(unsigned long index, void *buffer, unsigned long *length)
{
    if (!pCH375GetDeviceDescr) {
        return CH375_FALSE;
    }
    return pCH375GetDeviceDescr(index, buffer, length);
}

int ch375_reset_device(unsigned long index)
{
    if (!pCH375ResetDevice) {
        return CH375_FALSE;
    }
    return pCH375ResetDevice(index);
}

/* ============================================================================
 * 超时设置
 * ============================================================================ */

int ch375_set_timeout(unsigned long index, unsigned long writeTimeout, unsigned long readTimeout)
{
    if (!pCH375SetTimeout) {
        sr_err("CH375SetTimeout not loaded");
        return CH375_FALSE;
    }
    return pCH375SetTimeout(index, writeTimeout, readTimeout);
}

int ch375_set_timeout_ex(unsigned long index, unsigned long writeTimeout, unsigned long readTimeout,
                         unsigned long auxTimeout, unsigned long interTimeout)
{
    if (!pCH375SetTimeoutEx) {
        /* 回退到基本超时设置 */
        return ch375_set_timeout(index, writeTimeout, readTimeout);
    }
    return pCH375SetTimeoutEx(index, writeTimeout, readTimeout, auxTimeout, interTimeout);
}

/* ============================================================================
 * 端点1直接上下传 (命令传输)
 * ============================================================================ */

int ch375_write_read(unsigned long index, void *iBuffer, void *oBuffer, unsigned long *ioLength)
{
    if (!pCH375WriteRead) {
        sr_err("CH375WriteRead not loaded");
        return CH375_FALSE;
    }

    int result = pCH375WriteRead(index, iBuffer, oBuffer, ioLength);
    if (!result) {
        sr_dbg("CH375WriteRead failed at index %lu", index);
    }
    return result;
}

/* ============================================================================
 * 端点读写 (分开写读)
 * ============================================================================ */

int ch375_write_endpoint(unsigned long index, unsigned long endpoint, void *buffer, unsigned long *length)
{
    if (!pCH375WriteEndP) {
        sr_err("CH375WriteEndP not loaded");
        return CH375_FALSE;
    }
    return pCH375WriteEndP(index, endpoint, buffer, length);
}

int ch375_read_endpoint(unsigned long index, unsigned long endpoint, void *buffer, unsigned long *length)
{
    if (!pCH375ReadEndP) {
        sr_err("CH375ReadEndP not loaded");
        return CH375_FALSE;
    }
    return pCH375ReadEndP(index, endpoint, buffer, length);
}

/* ============================================================================
 * 端点控制
 * ============================================================================ */

int ch375_abort_endpoint_read(unsigned long index, unsigned long endpoint)
{
    if (!pCH375AbortEndPRead) {
        return CH375_FALSE;
    }
    return pCH375AbortEndPRead(index, endpoint);
}

int ch375_abort_endpoint_write(unsigned long index, unsigned long endpoint)
{
    if (!pCH375AbortEndPWrite) {
        return CH375_FALSE;
    }
    return pCH375AbortEndPWrite(index, endpoint);
}

int ch375_reset_in_endpoint(unsigned long index, unsigned long endpoint)
{
    if (!pCH375ResetInEndP) {
        return CH375_FALSE;
    }
    return pCH375ResetInEndP(index, endpoint);
}

int ch375_reset_out_endpoint(unsigned long index, unsigned long endpoint)
{
    if (!pCH375ResetOutEndP) {
        return CH375_FALSE;
    }
    return pCH375ResetOutEndP(index, endpoint);
}

/* ============================================================================
 * 缓冲上传模式 (端点2/3数据传输)
 * ============================================================================ */

int ch375_set_buf_upload_ex(unsigned long index, unsigned long enable, unsigned long endpoint, unsigned long transferSize)
{
    if (!pCH375SetBufUploadEx) {
        sr_err("CH375SetBufUploadEx not loaded");
        return CH375_FALSE;
    }

    int result = pCH375SetBufUploadEx(index, enable, endpoint, transferSize);
    if (result) {
        sr_dbg("Buffer upload %s for endpoint 0x%02lx, transfer size=%lu",
               enable ? "enabled" : "disabled", endpoint, transferSize);
    } else {
        sr_warn("Failed to set buffer upload for endpoint 0x%02lx", endpoint);
    }
    return result;
}

int ch375_query_buf_upload_ex(unsigned long index, unsigned long endpoint, unsigned long *transferCount, unsigned long *totalDataLen)
{
    if (!pCH375QueryBufUploadEx) {
        sr_err("CH375QueryBufUploadEx not loaded");
        return CH375_FALSE;
    }
    return pCH375QueryBufUploadEx(index, endpoint, transferCount, totalDataLen);
}

int ch375_clear_buf_upload(unsigned long index, unsigned long endpoint)
{
    if (!pCH375ClearBufUpload) {
        return CH375_FALSE;
    }
    return pCH375ClearBufUpload(index, endpoint);
}

/* ============================================================================
 * 缓冲下传模式
 * ============================================================================ */

int ch375_set_buf_download_ex(unsigned long index, unsigned long enable, unsigned long endpoint, unsigned long transferSize)
{
    if (!pCH375SetBufDownloadEx) {
        sr_err("CH375SetBufDownloadEx not loaded");
        return CH375_FALSE;
    }

    int result = pCH375SetBufDownloadEx(index, enable, endpoint, transferSize);
    if (result) {
        sr_dbg("Buffer download %s for endpoint 0x%02lx, transfer size=%lu",
               enable ? "enabled" : "disabled", endpoint, transferSize);
    } else {
        sr_warn("Failed to set buffer download for endpoint 0x%02lx", endpoint);
    }
    return result;
}

/* ============================================================================
 * IO模式设置
 * ============================================================================ */

// CH375SetIOMode函数实现（调用CH375DriverCommand）
int ch375_set_io_mode(unsigned long index, unsigned long sync)
{
    if (!pCH375DriverCommand) {
        sr_err("CH375DriverCommand not available");
        return CH375_FALSE;
    }

    mWIN32_COMMAND cmd;
    cmd.mFunction = mFuncSetIOMode;
    cmd.mLength = 1;
    cmd.mBuffer[0] = sync ? 1 : 0;
    return pCH375DriverCommand(index, &cmd);
}

/* ============================================================================
 * 数据端点读写
 * ============================================================================ */

int ch375_read_data(unsigned long index, void *buffer, unsigned long *length)
{
    if (!pCH375ReadData) {
        sr_err("CH375ReadData not loaded");
        return CH375_FALSE;
    }
    return pCH375ReadData(index, buffer, length);
}

int ch375_write_data(unsigned long index, void *buffer, unsigned long *length)
{
    if (!pCH375WriteData) {
        sr_err("CH375WriteData not loaded");
        return CH375_FALSE;
    }
    return pCH375WriteData(index, buffer, length);
}

int ch375_abort_read(unsigned long index)
{
    if (!pCH375AbortRead) {
        return CH375_FALSE;
    }
    return pCH375AbortRead(index);
}

int ch375_abort_write(unsigned long index)
{
    if (!pCH375AbortWrite) {
        return CH375_FALSE;
    }
    return pCH375AbortWrite(index);
}

/* ============================================================================
 * 设备事件通知
 * ============================================================================ */

int ch375_set_device_notify(unsigned long index, char *deviceID, CH375NotifyCallback callback)
{
    if (!pCH375SetDeviceNotify) {
        sr_err("CH375SetDeviceNotify not loaded");
        return CH375_FALSE;
    }

    int result = pCH375SetDeviceNotify(index, deviceID, callback);
    if (result) {
        if (callback) {
            sr_info("Device notify callback registered for index %lu", index);
        } else {
            sr_info("Device notify callback unregistered for index %lu", index);
        }
    } else {
        sr_warn("Failed to set device notify for index %lu", index);
    }
    return result;
}

#else /* !_WIN32 */

/*
 * ============================================================================
 * libusb 传输层实现（Linux / macOS）
 *
 * Windows 上 CH375DLL 由 WDM 驱动提供"缓冲上传"模式：驱动持续用大 URB 读取
 * 数据端点，应用通过 Query/Read 取走数据。这里用 libusb 异步传输实现同样的
 * 语义，不需要任何内核驱动：
 *
 *   - ch375_set_buf_upload_ex(1, pipe, size) 提交一个 size 字节的异步 IN 传输；
 *   - 传输完成后回调把数据块放入该管道的队列并重新提交；
 *   - ch375_query_buf_upload_ex 返回队列中的块数与总字节数；
 *   - ch375_read_endpoint 从队列取数据；命令端点（无异步读）退化为同步读。
 *
 * 端点约定与 CH375 一致：pipe N -> OUT 0x0N / IN 0x80+N，端点是批量或中断
 * 类型都按设备描述符实际类型处理。
 * ============================================================================
 */

#include <pthread.h>
#include <string.h>
#include <time.h>
#include <libusb.h>

#define CH375_USB_VID		0x1a86
#define CH375_USB_PID		0x5537
#define CH375_MAX_PIPES		8
#define CH375_QUEUE_DEPTH	16

struct ch375_pipe_queue {
	uint8_t *chunks[CH375_QUEUE_DEPTH];
	uint32_t chunk_len[CH375_QUEUE_DEPTH];
	uint8_t head;
	uint8_t count;
};

struct ch375_async_ctx {
	struct ch375_device *dev;
	uint8_t pipe;
};

struct ch375_device {
	libusb_device_handle *handle;
	libusb_device *dev;
	uint8_t out_ep[CH375_MAX_PIPES];
	uint8_t in_ep[CH375_MAX_PIPES];
	uint8_t out_ep_intr[CH375_MAX_PIPES];
	uint8_t in_ep_intr[CH375_MAX_PIPES];
	unsigned int timeout_ms;

	pthread_mutex_t lock;
	pthread_cond_t cond;

	struct ch375_pipe_queue queue;

	struct libusb_transfer *active[CH375_MAX_PIPES];
	uint32_t active_size[CH375_MAX_PIPES];
	uint8_t read_enabled[CH375_MAX_PIPES];
};

static libusb_context *ch375_ctx = NULL;
static struct ch375_device ch375_devs[CH375_MAX_NUMBER];
static uint8_t ch375_open_flags[CH375_MAX_NUMBER];
static pthread_t ch375_event_thread;
static volatile int ch375_event_running = 0;

static int ch375_submit_read(struct ch375_device *dev, uint8_t pipe);
int ch375_clear_buf_upload(unsigned long index, unsigned long endpoint);

static void *ch375_event_thread_func(void *arg)
{
	(void)arg;
	while (ch375_event_running) {
		struct timeval tv = { 0, 100000 };
		int r = libusb_handle_events_timeout(ch375_ctx, &tv);
		if (r < 0 && r != LIBUSB_ERROR_INTERRUPTED &&
		    r != LIBUSB_ERROR_TIMEOUT) {
			/* 事件循环出错，稍候重试 */
			g_usleep(10000);
		}
	}
	return NULL;
}

static void LIBUSB_CALL ch375_read_cb(struct libusb_transfer *tr)
{
	struct ch375_async_ctx *ac = (struct ch375_async_ctx *)tr->user_data;
	struct ch375_device *dev = ac->dev;
	uint8_t pipe = ac->pipe;
	uint8_t resubmit = 0;

	pthread_mutex_lock(&dev->lock);
	dev->active[pipe] = NULL;

	if (tr->status == LIBUSB_TRANSFER_COMPLETED && tr->actual_length > 0) {
		if (dev->queue.count < CH375_QUEUE_DEPTH) {
			uint32_t idx = (dev->queue.head + dev->queue.count) %
				CH375_QUEUE_DEPTH;
			dev->queue.chunks[idx] = tr->buffer;
			dev->queue.chunk_len[idx] = tr->actual_length;
			dev->queue.count++;
			tr->buffer = NULL;
			if (dev->read_enabled[pipe])
				resubmit = 1;
		} else {
			sr_warn("Pipe %u queue full, dropping %u bytes",
				pipe, tr->actual_length);
		}
	} else if (tr->status != LIBUSB_TRANSFER_CANCELLED) {
		sr_warn("Async read on pipe %u failed: %s",
			pipe, libusb_error_name(tr->status));
	}

	if (tr->buffer)
		g_free(tr->buffer);
	g_free(ac);
	libusb_free_transfer(tr);

	pthread_cond_broadcast(&dev->cond);
	pthread_mutex_unlock(&dev->lock);

	if (resubmit)
		ch375_submit_read(dev, pipe);
}

static int ch375_submit_read(struct ch375_device *dev, uint8_t pipe)
{
	struct libusb_transfer *tr;
	uint8_t *buf;
	struct ch375_async_ctx *ac;
	uint32_t size;
	int r;

	pthread_mutex_lock(&dev->lock);
	if (dev->active[pipe] || !dev->read_enabled[pipe] ||
	    !dev->in_ep[pipe]) {
		pthread_mutex_unlock(&dev->lock);
		return 0;
	}

	size = dev->active_size[pipe] ? dev->active_size[pipe] :
		(1024 * 1024);
	tr = libusb_alloc_transfer(0);
	buf = g_malloc(size);
	ac = g_malloc0(sizeof(*ac));
	ac->dev = dev;
	ac->pipe = pipe;

	if (dev->in_ep_intr[pipe])
		libusb_fill_interrupt_transfer(tr, dev->handle, dev->in_ep[pipe],
			buf, size, ch375_read_cb, ac, 0);
	else
		libusb_fill_bulk_transfer(tr, dev->handle, dev->in_ep[pipe],
			buf, size, ch375_read_cb, ac, 0);
	tr->timeout = 0;

	dev->active[pipe] = tr;
	r = libusb_submit_transfer(tr);
	if (r < 0) {
		dev->active[pipe] = NULL;
		pthread_mutex_unlock(&dev->lock);
		sr_warn("libusb_submit_transfer failed for pipe %u: %s",
			pipe, libusb_error_name(r));
		libusb_free_transfer(tr);
		g_free(buf);
		g_free(ac);
		return r;
	}
	pthread_mutex_unlock(&dev->lock);
	return 0;
}

static void ch375_stop_async(struct ch375_device *dev, uint8_t pipe)
{
	struct libusb_transfer *tr;

	pthread_mutex_lock(&dev->lock);
	dev->read_enabled[pipe] = 0;
	tr = dev->active[pipe];
	pthread_mutex_unlock(&dev->lock);

	if (tr) {
		libusb_cancel_transfer(tr);
		/* 等待回调清空 active 标志（事件线程处理） */
		pthread_mutex_lock(&dev->lock);
		while (dev->active[pipe]) {
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_sec += 1;
			if (pthread_cond_timedwait(&dev->cond, &dev->lock, &ts) != 0)
				break;
		}
		pthread_mutex_unlock(&dev->lock);
	}
}

static void ch375_setup_endpoints(struct ch375_device *dev,
				  libusb_device *usbdev)
{
	struct libusb_config_descriptor *cfg = NULL;
	int i;

	if (libusb_get_active_config_descriptor(usbdev, &cfg) < 0)
		return;

	for (i = 0; i < cfg->bNumInterfaces; i++) {
		const struct libusb_interface *iface = &cfg->interface[i];
		int j;
		if (i != 0)
			continue;
		for (j = 0; j < iface->num_altsetting; j++) {
			const struct libusb_interface_descriptor *alt =
				&iface->altsetting[j];
			int k;
			for (k = 0; k < alt->bNumEndpoints; k++) {
				const struct libusb_endpoint_descriptor *ep =
					&alt->endpoint[k];
				uint8_t addr = ep->bEndpointAddress;
				uint8_t pipe = addr & 0x0F;
				uint8_t intr =
					((ep->bmAttributes & 0x03) ==
					 LIBUSB_TRANSFER_TYPE_INTERRUPT);
				if (pipe == 0 || pipe >= CH375_MAX_PIPES)
					continue;
				if (addr & 0x80) {
					dev->in_ep[pipe] = addr;
					dev->in_ep_intr[pipe] = intr;
				} else {
					dev->out_ep[pipe] = addr;
					dev->out_ep_intr[pipe] = intr;
				}
			}
		}
	}
	libusb_free_config_descriptor(cfg);
}

int ch375_init(void)
{
	if (ch375_ctx)
		return SR_OK;

	if (libusb_init(&ch375_ctx) < 0) {
		sr_err("libusb_init failed");
		return SR_ERR;
	}

	memset(ch375_devs, 0, sizeof(ch375_devs));
	memset(ch375_open_flags, 0, sizeof(ch375_open_flags));

	ch375_event_running = 1;
	if (pthread_create(&ch375_event_thread, NULL,
			   ch375_event_thread_func, NULL) != 0) {
		sr_err("Failed to create libusb event thread");
		ch375_event_running = 0;
		libusb_exit(ch375_ctx);
		ch375_ctx = NULL;
		return SR_ERR;
	}

	return SR_OK;
}

void ch375_cleanup(void)
{
	int i;

	for (i = 0; i < CH375_MAX_NUMBER; i++)
		if (ch375_open_flags[i])
			ch375_close_device(i);

	if (ch375_ctx) {
		ch375_event_running = 0;
		pthread_join(ch375_event_thread, NULL);
		libusb_exit(ch375_ctx);
		ch375_ctx = NULL;
	}
}

int ch375_is_loaded(void)
{
	return (ch375_ctx != NULL) ? CH375_TRUE : CH375_FALSE;
}

void *ch375_open_device(unsigned long index)
{
	libusb_device **list;
	ssize_t n, k;
	ssize_t match = 0;
	libusb_device *usbdev = NULL;
	struct ch375_device *dev;

	if (index >= CH375_MAX_NUMBER || !ch375_ctx)
		return CH375_INVALID_HANDLE;

	if (ch375_open_flags[index])
		return &ch375_devs[index];

	n = libusb_get_device_list(ch375_ctx, &list);
	if (n < 0)
		return CH375_INVALID_HANDLE;

	for (k = 0; k < n; k++) {
		struct libusb_device_descriptor desc;
		if (libusb_get_device_descriptor(list[k], &desc) < 0)
			continue;
		if (desc.idVendor == CH375_USB_VID &&
		    desc.idProduct == CH375_USB_PID) {
			if (match == (ssize_t)index) {
				usbdev = list[k];
				break;
			}
			match++;
		}
	}

	if (!usbdev) {
		libusb_free_device_list(list, 1);
		return CH375_INVALID_HANDLE;
	}

	dev = &ch375_devs[index];
	memset(dev, 0, sizeof(*dev));
	dev->timeout_ms = 500;
	pthread_mutex_init(&dev->lock, NULL);
	pthread_cond_init(&dev->cond, NULL);

	libusb_ref_device(usbdev);
	dev->dev = usbdev;

	if (libusb_open(usbdev, &dev->handle) < 0) {
		sr_err("libusb_open failed for device %lu", index);
		libusb_unref_device(usbdev);
		return CH375_INVALID_HANDLE;
	}

	/* 配置/声明接口失败只告警，不阻断 */
	if (libusb_set_configuration(dev->handle, 1) < 0)
		sr_dbg("libusb_set_configuration(1) failed for device %lu", index);
	if (libusb_claim_interface(dev->handle, 0) < 0)
		sr_warn("Failed to claim interface 0 for device %lu", index);

	ch375_setup_endpoints(dev, usbdev);
	ch375_open_flags[index] = 1;

	libusb_free_device_list(list, 1);
	return dev;
}

void ch375_close_device(unsigned long index)
{
	struct ch375_device *dev;
	int pipe;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return;

	dev = &ch375_devs[index];

	for (pipe = 1; pipe < CH375_MAX_PIPES; pipe++)
		ch375_stop_async(dev, (uint8_t)pipe);

	pthread_mutex_lock(&dev->lock);
	while (dev->queue.count > 0) {
		g_free(dev->queue.chunks[dev->queue.head]);
		dev->queue.head = (dev->queue.head + 1) % CH375_QUEUE_DEPTH;
		dev->queue.count--;
	}
	pthread_mutex_unlock(&dev->lock);

	if (dev->handle) {
		libusb_release_interface(dev->handle, 0);
		libusb_close(dev->handle);
	}
	if (dev->dev)
		libusb_unref_device(dev->dev);

	pthread_mutex_destroy(&dev->lock);
	pthread_cond_destroy(&dev->cond);
	ch375_open_flags[index] = 0;
}

unsigned long ch375_get_usb_id(unsigned long index)
{
	struct libusb_device_descriptor desc;
	struct ch375_device *dev;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return 0;
	dev = &ch375_devs[index];

	if (libusb_get_device_descriptor(dev->dev, &desc) < 0)
		return 0;

	/* 与 CH375DLL 一致：高位 PID，低位 VID */
	return ((unsigned long)desc.idProduct << 16) | desc.idVendor;
}

const char *ch375_get_device_name(unsigned long index)
{
	(void)index;
	return "CH32H417 Logic Analyzer";
}

int ch375_get_device_descr(unsigned long index, void *buffer,
			   unsigned long *length)
{
	struct libusb_device_descriptor d;
	struct ch375_device *dev;
	uint8_t *b = (uint8_t *)buffer;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index] ||
	    !buffer || !length)
		return CH375_FALSE;
	dev = &ch375_devs[index];

	if (libusb_get_device_descriptor(dev->dev, &d) < 0)
		return CH375_FALSE;

	b[0] = 0x12;
	b[1] = 0x01;
	b[2] = d.bcdUSB & 0xff;
	b[3] = d.bcdUSB >> 8;
	b[4] = d.bDeviceClass;
	b[5] = d.bDeviceSubClass;
	b[6] = d.bDeviceProtocol;
	b[7] = d.bMaxPacketSize0;
	b[8] = d.idVendor & 0xff;
	b[9] = d.idVendor >> 8;
	b[10] = d.idProduct & 0xff;
	b[11] = d.idProduct >> 8;
	b[12] = d.bcdDevice & 0xff;
	b[13] = d.bcdDevice >> 8;
	b[14] = d.iManufacturer;
	b[15] = d.iProduct;
	b[16] = d.iSerialNumber;
	b[17] = d.bNumConfigurations;
	*length = 18;

	return CH375_TRUE;
}

int ch375_reset_device(unsigned long index)
{
	struct ch375_device *dev;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return CH375_FALSE;
	dev = &ch375_devs[index];

	return (libusb_reset_device(dev->handle) == 0) ?
		CH375_TRUE : CH375_FALSE;
}

int ch375_set_timeout(unsigned long index, unsigned long writeTimeout,
		      unsigned long readTimeout)
{
	struct ch375_device *dev;
	(void)writeTimeout;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return CH375_FALSE;
	dev = &ch375_devs[index];
	dev->timeout_ms = readTimeout;
	return CH375_TRUE;
}

int ch375_set_timeout_ex(unsigned long index, unsigned long writeTimeout,
			 unsigned long readTimeout,
			 unsigned long writeTimeout2,
			 unsigned long readTimeout2)
{
	struct ch375_device *dev;
	(void)writeTimeout;
	(void)writeTimeout2;
	(void)readTimeout2;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return CH375_FALSE;
	dev = &ch375_devs[index];
	dev->timeout_ms = readTimeout;
	return CH375_TRUE;
}

int ch375_write_read(unsigned long index, void *iBuffer, void *oBuffer,
		     unsigned long *ioLength)
{
	unsigned long len;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index] ||
	    !iBuffer || !oBuffer || !ioLength)
		return CH375_FALSE;

	len = *ioLength;
	if (!ch375_write_endpoint(index, 1, iBuffer, &len))
		return CH375_FALSE;

	len = *ioLength;
	if (!ch375_read_endpoint(index, 1, oBuffer, &len))
		return CH375_FALSE;
	*ioLength = len;
	return CH375_TRUE;
}

int ch375_write_endpoint(unsigned long index, unsigned long endpoint,
			 void *buffer, unsigned long *length)
{
	struct ch375_device *dev;
	int ep, transferred = 0, r;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index] ||
	    !buffer || !length)
		return CH375_FALSE;
	if (endpoint >= CH375_MAX_PIPES)
		return CH375_FALSE;
	dev = &ch375_devs[index];

	ep = dev->out_ep[endpoint] ? dev->out_ep[endpoint] :
		(int)(0x01 + endpoint);

	if (dev->out_ep_intr[endpoint])
		r = libusb_interrupt_transfer(dev->handle, (uint8_t)ep, buffer,
			(int)*length, &transferred, dev->timeout_ms);
	else
		r = libusb_bulk_transfer(dev->handle, (uint8_t)ep, buffer,
			(int)*length, &transferred, dev->timeout_ms);

	if (r == 0) {
		*length = transferred;
		return CH375_TRUE;
	}
	sr_warn("Sync OUT ep 0x%02x failed: %s", ep, libusb_error_name(r));
	return CH375_FALSE;
}

int ch375_read_endpoint(unsigned long index, unsigned long endpoint,
			void *buffer, unsigned long *length)
{
	struct ch375_device *dev;
	int ep, transferred = 0, r;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index] ||
	    !buffer || !length)
		return CH375_FALSE;
	if (endpoint >= CH375_MAX_PIPES)
		return CH375_FALSE;
	dev = &ch375_devs[index];

	/* 数据管道：优先从异步读队列取 */
	pthread_mutex_lock(&dev->lock);
	if (dev->queue.count == 0 && dev->read_enabled[endpoint] &&
	    dev->active[endpoint]) {
		/* 异步读在途，等待最多 100ms */
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_nsec += 100000000;
		if (ts.tv_nsec >= 1000000000) {
			ts.tv_sec++;
			ts.tv_nsec -= 1000000000;
		}
		while (dev->queue.count == 0 && dev->active[endpoint])
			if (pthread_cond_timedwait(&dev->cond, &dev->lock,
						   &ts) != 0)
				break;
	}
	if (dev->queue.count > 0) {
		uint32_t clen = dev->queue.chunk_len[dev->queue.head];
		uint32_t n = (clen < (uint32_t)*length) ?
			clen : (uint32_t)*length;
		uint8_t resubmit;

		memcpy(buffer, dev->queue.chunks[dev->queue.head], n);
		g_free(dev->queue.chunks[dev->queue.head]);
		dev->queue.head = (dev->queue.head + 1) % CH375_QUEUE_DEPTH;
		dev->queue.count--;
		*length = n;
		resubmit = dev->read_enabled[endpoint] &&
			!dev->active[endpoint];
		pthread_mutex_unlock(&dev->lock);
		if (resubmit)
			ch375_submit_read(dev, (uint8_t)endpoint);
		return CH375_TRUE;
	}
	if (dev->read_enabled[endpoint]) {
		/* 有异步读在跑但暂无数据：交给上层稍后重试 */
		pthread_mutex_unlock(&dev->lock);
		return CH375_FALSE;
	}
	pthread_mutex_unlock(&dev->lock);

	/* 命令端点：同步读 */
	ep = dev->in_ep[endpoint] ? dev->in_ep[endpoint] :
		(int)(0x80 + endpoint);
	if (dev->in_ep_intr[endpoint])
		r = libusb_interrupt_transfer(dev->handle, (uint8_t)ep, buffer,
			(int)*length, &transferred, dev->timeout_ms);
	else
		r = libusb_bulk_transfer(dev->handle, (uint8_t)ep, buffer,
			(int)*length, &transferred, dev->timeout_ms);

	if (r == 0) {
		*length = transferred;
		return CH375_TRUE;
	}
	sr_warn("Sync IN ep 0x%02x failed: %s", ep, libusb_error_name(r));
	return CH375_FALSE;
}

int ch375_abort_endpoint_read(unsigned long index, unsigned long endpoint)
{
	(void)endpoint;
	return ch375_clear_buf_upload(index, endpoint);
}

int ch375_abort_endpoint_write(unsigned long index, unsigned long endpoint)
{
	(void)index;
	(void)endpoint;
	return CH375_TRUE;
}

int ch375_reset_in_endpoint(unsigned long index, unsigned long endpoint)
{
	struct ch375_device *dev;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return CH375_FALSE;
	dev = &ch375_devs[index];

	if (dev->in_ep[endpoint])
		libusb_clear_halt(dev->handle, dev->in_ep[endpoint]);
	return CH375_TRUE;
}

int ch375_reset_out_endpoint(unsigned long index, unsigned long endpoint)
{
	struct ch375_device *dev;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return CH375_FALSE;
	dev = &ch375_devs[index];

	if (dev->out_ep[endpoint])
		libusb_clear_halt(dev->handle, dev->out_ep[endpoint]);
	return CH375_TRUE;
}

int ch375_set_buf_upload_ex(unsigned long index, unsigned long enable,
			    unsigned long endpoint, unsigned long transferSize)
{
	struct ch375_device *dev;
	uint8_t need_submit = 0;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return CH375_FALSE;
	if (endpoint >= CH375_MAX_PIPES)
		return CH375_FALSE;
	dev = &ch375_devs[index];

	if (enable) {
		pthread_mutex_lock(&dev->lock);
		dev->active_size[endpoint] = (transferSize > 0) ?
			transferSize : (1024 * 1024);
		dev->read_enabled[endpoint] = 1;
		need_submit = !dev->active[endpoint];
		pthread_mutex_unlock(&dev->lock);
		if (need_submit)
			ch375_submit_read(dev, (uint8_t)endpoint);
	} else {
		ch375_stop_async(dev, (uint8_t)endpoint);
	}
	return CH375_TRUE;
}

int ch375_query_buf_upload_ex(unsigned long index, unsigned long endpoint,
			      unsigned long *transferCount,
			      unsigned long *totalDataLen)
{
	struct ch375_device *dev;
	uint32_t i, total = 0;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index] ||
	    !transferCount || !totalDataLen)
		return CH375_FALSE;
	dev = &ch375_devs[index];

	pthread_mutex_lock(&dev->lock);
	*transferCount = dev->queue.count;
	for (i = 0; i < dev->queue.count; i++)
		total += dev->queue.chunk_len[
			(dev->queue.head + i) % CH375_QUEUE_DEPTH];
	*totalDataLen = total;
	pthread_mutex_unlock(&dev->lock);
	return CH375_TRUE;
}

int ch375_clear_buf_upload(unsigned long index, unsigned long endpoint)
{
	struct ch375_device *dev;
	(void)endpoint;

	if (index >= CH375_MAX_NUMBER || !ch375_open_flags[index])
		return CH375_FALSE;
	dev = &ch375_devs[index];

	pthread_mutex_lock(&dev->lock);
	while (dev->queue.count > 0) {
		g_free(dev->queue.chunks[dev->queue.head]);
		dev->queue.head = (dev->queue.head + 1) % CH375_QUEUE_DEPTH;
		dev->queue.count--;
	}
	pthread_mutex_unlock(&dev->lock);
	return CH375_TRUE;
}

int ch375_set_buf_download_ex(unsigned long index, unsigned long enable,
			      unsigned long endpoint, unsigned long transferSize)
{
	(void)index;
	(void)enable;
	(void)endpoint;
	(void)transferSize;
	/* 下传始终走同步写，无需缓冲 */
	return CH375_TRUE;
}

int ch375_set_io_mode(unsigned long index, unsigned long sync)
{
	(void)index;
	(void)sync;
	/* libusb 同步传输本身就是同步语义 */
	return CH375_TRUE;
}

int ch375_read_data(unsigned long index, void *buffer, unsigned long *length)
{
	return ch375_read_endpoint(index, 2, buffer, length);
}

int ch375_write_data(unsigned long index, void *buffer, unsigned long *length)
{
	return ch375_write_endpoint(index, 2, buffer, length);
}

int ch375_abort_read(unsigned long index)
{
	return ch375_clear_buf_upload(index, 2);
}

int ch375_abort_write(unsigned long index)
{
	(void)index;
	return CH375_TRUE;
}

int ch375_set_device_notify(unsigned long index, char *deviceID,
			    CH375NotifyCallback callback)
{
	(void)index;
	(void)deviceID;
	(void)callback;
	/* 热插拔通知由上层（pulseview 的 libusb 轮询）处理 */
	return CH375_TRUE;
}

#endif /* _WIN32 */

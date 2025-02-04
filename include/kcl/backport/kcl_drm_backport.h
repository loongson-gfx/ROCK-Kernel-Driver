/* SPDX-License-Identifier: MIT */
#ifndef AMDKCL_DRM_BACKPORT_H
#define AMDKCL_DRM_BACKPORT_H

#include <linux/ctype.h>
#include <drm/drm_fourcc.h>
#include <kcl/kcl_drm.h>
#include <kcl/header/kcl_drm_file_h.h>
#if defined(HAVE_CHUNK_ID_SYNOBJ_IN_OUT)
#include <drm/drm_syncobj.h>
#endif

#if !defined(HAVE_DRM_GET_FORMAT_NAME_I_P)
/**
 * struct drm_format_name_buf - name of a DRM format
 * @str: string buffer containing the format name
 */
struct drm_format_name_buf {
	char str[32];
};

static char printable_char(int c)
{
	return isascii(c) && isprint(c) ? c : '?';
}

static inline const char *_kcl_drm_get_format_name(uint32_t format, struct drm_format_name_buf *buf)
{
	snprintf(buf->str, sizeof(buf->str),
		 "%c%c%c%c %s-endian (0x%08x)",
		 printable_char(format & 0xff),
		 printable_char((format >> 8) & 0xff),
		 printable_char((format >> 16) & 0xff),
		 printable_char((format >> 24) & 0x7f),
		 format & DRM_FORMAT_BIG_ENDIAN ? "big" : "little",
		 format);

	return buf->str;
}
#define drm_get_format_name _kcl_drm_get_format_name
#endif

#if defined(HAVE_CHUNK_ID_SYNOBJ_IN_OUT)
static inline
int _kcl_drm_syncobj_find_fence(struct drm_file *file_private,
						u32 handle, u64 point, u64 flags,
						struct dma_fence **fence)
{
#if defined(HAVE_DRM_SYNCOBJ_FIND_FENCE)
#if defined(HAVE_DRM_SYNCOBJ_FIND_FENCE_5ARGS)
	return drm_syncobj_find_fence(file_private, handle, point, flags, fence);
#elif defined(HAVE_DRM_SYNCOBJ_FIND_FENCE_4ARGS)
	return drm_syncobj_find_fence(file_private, handle, point, fence);
#else
	return drm_syncobj_find_fence(file_private, handle, fence);
#endif
#elif defined(HAVE_DRM_SYNCOBJ_FENCE_GET)
	return drm_syncobj_fence_get(file_private, handle, fence);
#endif
}
#define drm_syncobj_find_fence _kcl_drm_syncobj_find_fence
#endif

/*
 * commit d3252ace0bc652a1a244455556b6a549f969bf99
 * PCI: Restore resized BAR state on resume
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
#define AMDKCL_ENABLE_RESIZE_FB_BAR
#endif

/*
 * commit v4.10-rc3-539-g086f2e5cde74
 * drm: debugfs: Remove all files automatically on cleanup
 */
#if DRM_VERSION_CODE < DRM_VERSION(4, 11, 0)
#define AMDKCL_AMDGPU_DEBUGFS_CLEANUP
#endif

#ifndef HAVE_DRM_GEM_OBJECT_LOOKUP_2ARGS
static inline struct drm_gem_object *
_kcl_drm_gem_object_lookup(struct drm_file *filp, u32 handle)
{
	return drm_gem_object_lookup(filp->minor->dev, filp, handle);
}
#define drm_gem_object_lookup _kcl_drm_gem_object_lookup
#endif

#if DRM_VERSION_CODE >= DRM_VERSION(4, 17, 0)
#define AMDKCL_AMDGPU_DMABUF_OPS
#endif

/*
 * commit v5.4-rc4-1120-gb3fac52c5193
 * drm: share address space for dma bufs
 */
#if DRM_VERSION_CODE < DRM_VERSION(5, 5, 0)
#define AMDKCL_DMA_BUF_SHARE_ADDR_SPACE
#endif


#ifdef HAVE_DRM_DEV_UNPLUG
/*
 * v5.1-rc5-1150-gbd53280ef042 drm/drv: Fix incorrect resolution of merge conflict
 * v5.1-rc2-5-g3f04e0a6cfeb drm: Fix drm_release() and device unplug
 */
#if DRM_VERSION_CODE < DRM_VERSION(5, 2, 0)
static inline
void _kcl_drm_dev_unplug(struct drm_device *dev)
{
	unsigned int prev, post;

	drm_dev_get(dev);

	prev = kref_read(&dev->ref);
	drm_dev_unplug(dev);
	post = kref_read(&dev->ref);

	if (prev == post)
		drm_dev_put(dev);
}
#define drm_dev_unplug _kcl_drm_dev_unplug
#endif
#endif

#endif/*AMDKCL_DRM_BACKPORT_H*/

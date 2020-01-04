/*
 * Copyright (C) 2016-2019 Doubango AI <https://www.doubango.org>
 * License: For non-commercial use only
 * Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
 * WebSite: https://www.doubango.org/webapps/mrz/
 */
package org.doubango.ultimateMrz.common;

import android.media.Image;

import java.util.concurrent.atomic.AtomicInteger;

public class MrzImage {

    Image mImage;
    final AtomicInteger mRefCount;

    private MrzImage(final Image image) {
        assert image != null;
        mImage = image;
        mRefCount = new AtomicInteger(0);
    }

    public static MrzImage newInstance(final Image image) {
        return new MrzImage(image);
    }

    public final Image getImage() {
        assert mRefCount.intValue() >= 0;
        return mImage;
    }

    public MrzImage takeRef() {
        assert mRefCount.intValue() >= 0;
        if (mRefCount.intValue() < 0) {
            return null;
        }
        mRefCount.incrementAndGet();
        return this;
    }

    public void releaseRef() {
        assert mRefCount.intValue() >= 0;
        final int refCount = mRefCount.decrementAndGet();
        if (refCount <= 0) {
            mImage.close();
            mImage = null;
        }
    }

    @Override
    protected synchronized void finalize() {
        if (mImage != null && mRefCount.intValue() < 0) {
            mImage.close();
        }
    }
}
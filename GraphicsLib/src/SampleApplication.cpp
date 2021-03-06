//
// Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "SampleApplication.h"
#include "EGLWindow.h"
#include "OSWindow.h"

SampleApplication::SampleApplication(const std::string& name, size_t width, size_t height,
                                     EGLint glesMajorVersion, EGLint requestedRenderer)
    : mName(name),
      mRunning(false)
{
    mEGLWindow.reset(new EGLWindow(width, height, glesMajorVersion, requestedRenderer));
    mTimer.reset(CreateTimer());
    mOSWindow.reset(CreateOSWindow());
}

SampleApplication::~SampleApplication()
{
}

bool SampleApplication::initialize()
{
    return true;
}

void SampleApplication::destroy()
{
}

void SampleApplication::step(float dt, double totalTime)
{
}

void SampleApplication::draw()
{
}

void SampleApplication::swap()
{
    mEGLWindow->swap();
}

OSWindow *SampleApplication::getWindow() const
{
    return mOSWindow.get();
}

EGLConfig SampleApplication::getConfig() const
{
    return mEGLWindow->getConfig();
}

EGLDisplay SampleApplication::getDisplay() const
{
    return mEGLWindow->getDisplay();
}

EGLSurface SampleApplication::getSurface() const
{
    return mEGLWindow->getSurface();
}

EGLContext SampleApplication::getContext() const
{
    return mEGLWindow->getContext();
}


int SampleApplication::run()
{
    if (!mOSWindow->initialize(mName, mEGLWindow->getWidth(), mEGLWindow->getHeight()))
    {
        return -1;
    }

    if (!mEGLWindow->initializeGL(mOSWindow.get()))
    {
        return -1;
    }

    mRunning = true;
    int result = 0;

    if (!initialize())
    {
        mRunning = false;
        result = -1;
    }

    mTimer->start();
    double prevTime = 0.0;

    while (mRunning)
    {
        double elapsedTime = mTimer->getElapsedTime();
        double deltaTime = elapsedTime - prevTime;

        step(static_cast<float>(deltaTime), elapsedTime);

        // Clear events that the application did not process from this frame
        Event event;
		while (popEvent(&event))
		{
			handleEvent(&event);
			switch (event.Type)
			{				
				case Event::EVENT_CLOSED:
				{
					exit();
				}
			}
        }

        if (!mRunning)
        {
            break;
        }

        draw();
        swap();

        mOSWindow->messageLoop();

        prevTime = elapsedTime;
    }

    destroy();
    mEGLWindow->destroyGL();
    mOSWindow->destroy();

    return result;
}

void SampleApplication::exit()
{
    mRunning = false;
}

bool SampleApplication::popEvent(Event *event)
{
    return mOSWindow->popEvent(event);
}

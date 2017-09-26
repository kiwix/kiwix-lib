/*
 * Copyright (C) 2013 Emmanuel Engelhart <kelson@kiwix.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <jni.h>
#include "org_kiwix_kiwixlib_JNIKiwix.h"

#include <stdio.h>
#include <string.h>

#include <iostream>
#include <string>

#include "common/base64.h"
#include "reader.h"
#include "searcher.h"
#include "unicode/putil.h"

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "kiwix", __VA_ARGS__)

#include <xapian.h>
#include <zim/article.h>
#include <zim/error.h>
#include <zim/file.h>
#include <zim/zim.h>

/* global variables */
kiwix::Reader* reader = NULL;
kiwix::Searcher* searcher = NULL;

static pthread_mutex_t readerLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t searcherLock = PTHREAD_MUTEX_INITIALIZER;

/* c2jni type conversion functions */
jboolean c2jni(const bool& val)
{
  return val ? JNI_TRUE : JNI_FALSE;
}
jstring c2jni(const std::string& val, JNIEnv* env)
{
  return env->NewStringUTF(val.c_str());
}

jint c2jni(const int val)
{
  return (jint)val;
}
jint c2jni(const unsigned val)
{
  return (unsigned)val;
}
/* jni2c type conversion functions */
bool jni2c(const jboolean& val)
{
  return val == JNI_TRUE;
}
std::string jni2c(const jstring& val, JNIEnv* env)
{
  return std::string(env->GetStringUTFChars(val, 0));
}

int jni2c(const jint val)
{
  return (int)val;
}
/* Method to deal with variable passed by reference */
void setStringObjValue(const std::string& value, const jobject obj, JNIEnv* env)
{
  jclass objClass = env->GetObjectClass(obj);
  jfieldID objFid = env->GetFieldID(objClass, "value", "Ljava/lang/String;");
  env->SetObjectField(obj, objFid, c2jni(value, env));
}

void setIntObjValue(const int value, const jobject obj, JNIEnv* env)
{
  jclass objClass = env->GetObjectClass(obj);
  jfieldID objFid = env->GetFieldID(objClass, "value", "I");
  env->SetIntField(obj, objFid, value);
}

void setBoolObjValue(const bool value, const jobject obj, JNIEnv* env)
{
  jclass objClass = env->GetObjectClass(obj);
  jfieldID objFid = env->GetFieldID(objClass, "value", "Z");
  env->SetIntField(obj, objFid, c2jni(value));
}

/* Kiwix library functions */
JNIEXPORT jstring JNICALL
Java_org_kiwix_kiwixlib_JNIKiwix_getMainPage(JNIEnv* env, jobject obj)
{
  jstring url;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cUrl = reader->getMainPageUrl();
      url = c2jni(cUrl, env);
    } catch (...) {
      std::cerr << "Unable to get ZIM main page" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return url;
}

JNIEXPORT jstring JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getId(JNIEnv* env,
                                                                 jobject obj)
{
  jstring id;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cId = reader->getId();
      id = c2jni(cId, env);
    } catch (...) {
      std::cerr << "Unable to get ZIM id" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return id;
}

JNIEXPORT jint JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getFileSize(JNIEnv* env,
                                                                    jobject obj)
{
  jint size;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      int cSize = reader->getFileSize();
      size = c2jni(cSize);
    } catch (...) {
      std::cerr << "Unable to get ZIM file size" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return size;
}

JNIEXPORT jstring JNICALL
Java_org_kiwix_kiwixlib_JNIKiwix_getCreator(JNIEnv* env, jobject obj)
{
  jstring creator;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cCreator = reader->getCreator();
      creator = c2jni(cCreator, env);
    } catch (...) {
      std::cerr << "Unable to get ZIM creator" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return creator;
}

JNIEXPORT jstring JNICALL
Java_org_kiwix_kiwixlib_JNIKiwix_getPublisher(JNIEnv* env, jobject obj)
{
  jstring publisher;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cPublisher = reader->getPublisher();
      publisher = c2jni(cPublisher, env);
    } catch (...) {
      std::cerr << "Unable to get ZIM creator" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return publisher;
}

JNIEXPORT jstring JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getName(JNIEnv* env,
                                                                   jobject obj)
{
  jstring name;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cName = reader->getName();
      name = c2jni(cName, env);
    } catch (...) {
      std::cerr << "Unable to get ZIM name" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return name;
}

JNIEXPORT jstring JNICALL
Java_org_kiwix_kiwixlib_JNIKiwix_getFavicon(JNIEnv* env, jobject obj)
{
  jstring favicon;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cContent;
      std::string cMime;
      reader->getFavicon(cContent, cMime);
      favicon
          = c2jni(base64_encode(
                      reinterpret_cast<const unsigned char*>(cContent.c_str()),
                      cContent.length()),
                  env);
    } catch (...) {
      std::cerr << "Unable to get ZIM favicon" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return favicon;
}

JNIEXPORT jstring JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getDate(JNIEnv* env,
                                                                   jobject obj)
{
  jstring date;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cDate = reader->getDate();
      date = c2jni(cDate, env);
    } catch (...) {
      std::cerr << "Unable to get ZIM date" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return date;
}

JNIEXPORT jstring JNICALL
Java_org_kiwix_kiwixlib_JNIKiwix_getLanguage(JNIEnv* env, jobject obj)
{
  jstring language;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cLanguage = reader->getLanguage();
      language = c2jni(cLanguage, env);
    } catch (...) {
      std::cerr << "Unable to get ZIM language" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return language;
}

JNIEXPORT jstring JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getMimeType(
    JNIEnv* env, jobject obj, jstring url)
{
  jstring mimeType;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    std::string cUrl = jni2c(url, env);
    try {
      std::string cMimeType;
      reader->getMimeTypeByUrl(cUrl, cMimeType);
      mimeType = c2jni(cMimeType, env);
    } catch (...) {
      std::cerr << "Unable to get mime-type for url " << cUrl << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return mimeType;
}

JNIEXPORT jboolean JNICALL
Java_org_kiwix_kiwixlib_JNIKiwix_loadZIM(JNIEnv* env, jobject obj, jstring path)
{
  jboolean retVal = JNI_TRUE;
  std::string cPath = jni2c(path, env);

  pthread_mutex_lock(&readerLock);
  try {
    if (reader != NULL) {
      delete reader;
    }
    reader = new kiwix::Reader(cPath);
  } catch (...) {
    std::cerr << "Unable to load ZIM " << cPath << std::endl;
    reader = NULL;
    retVal = JNI_FALSE;
  }
  pthread_mutex_unlock(&readerLock);

  return retVal;
}

JNIEXPORT jbyteArray JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getContent(
    JNIEnv* env, jobject obj, jstring url, jobject titleObj, jobject mimeTypeObj, jobject sizeObj)
{
  /* Default values */
  setStringObjValue("", titleObj, env);
  setStringObjValue("", mimeTypeObj, env);
  setIntObjValue(0, sizeObj, env);
  jbyteArray data = env->NewByteArray(0);

  /* Retrieve the content */
  if (reader != NULL) {
    std::string cUrl = jni2c(url, env);
    std::string cData;
    std::string cTitle;
    std::string cMimeType;
    unsigned int cSize = 0;

    pthread_mutex_lock(&readerLock);
    try {
      if (reader->getContentByUrl(cUrl, cData, cTitle, cSize, cMimeType)) {
        data = env->NewByteArray(cSize);
        env->SetByteArrayRegion(
            data, 0, cSize, reinterpret_cast<const jbyte*>(cData.c_str()));
        setStringObjValue(cMimeType, mimeTypeObj, env);
        setStringObjValue(cTitle, titleObj, env);
        setIntObjValue(cSize, sizeObj, env);
      }
    } catch (...) {
      std::cerr << "Unable to get content for url " << cUrl << std::endl;
    }
    pthread_mutex_unlock(&readerLock);
  }

  return data;
}

JNIEXPORT jboolean JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_searchSuggestions(
    JNIEnv* env, jobject obj, jstring prefix, jint count)
{
  jboolean retVal = JNI_FALSE;
  std::string cPrefix = jni2c(prefix, env);
  unsigned int cCount = jni2c(count);

  pthread_mutex_lock(&readerLock);
  try {
    if (reader != NULL) {
      if (reader->searchSuggestionsSmart(cPrefix, cCount)) {
        retVal = JNI_TRUE;
      }
    }
  } catch (...) {
    std::cerr << "Unable to search suggestions for pattern " << cPrefix
              << std::endl;
  }
  pthread_mutex_unlock(&readerLock);

  return retVal;
}

JNIEXPORT jboolean JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getNextSuggestion(
    JNIEnv* env, jobject obj, jobject titleObj)
{
  jboolean retVal = JNI_FALSE;
  std::string cTitle;

  pthread_mutex_lock(&readerLock);
  try {
    if (reader != NULL) {
      if (reader->getNextSuggestion(cTitle)) {
        setStringObjValue(cTitle, titleObj, env);
        retVal = JNI_TRUE;
      }
    }
  } catch (...) {
    std::cerr << "Unable to get next suggestion" << std::endl;
  }
  pthread_mutex_unlock(&readerLock);

  return retVal;
}

JNIEXPORT jboolean JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getPageUrlFromTitle(
    JNIEnv* env, jobject obj, jstring title, jobject urlObj)
{
  jboolean retVal = JNI_FALSE;
  std::string cTitle = jni2c(title, env);
  std::string cUrl;

  pthread_mutex_lock(&readerLock);
  try {
    if (reader != NULL) {
      if (reader->getPageUrlFromTitle(cTitle, cUrl)) {
        setStringObjValue(cUrl, urlObj, env);
        retVal = JNI_TRUE;
      }
    }
  } catch (...) {
    std::cerr << "Unable to get URL for title " << cTitle << std::endl;
  }
  pthread_mutex_unlock(&readerLock);

  return retVal;
}

JNIEXPORT jboolean JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getTitle(
    JNIEnv* env, jobject obj, jobject titleObj)
{
  jboolean retVal = JNI_FALSE;
  std::string cTitle;

  pthread_mutex_lock(&readerLock);
  try {
    if (reader != NULL) {
      std::string cTitle = reader->getTitle();
      setStringObjValue(cTitle, titleObj, env);
      retVal = JNI_TRUE;
    }
  } catch (...) {
    std::cerr << "Unable to get ZIM title" << std::endl;
  }
  pthread_mutex_unlock(&readerLock);

  return retVal;
}

JNIEXPORT jstring JNICALL
Java_org_kiwix_kiwixlib_JNIKiwix_getDescription(JNIEnv* env, jobject obj)
{
  jstring description;

  pthread_mutex_lock(&readerLock);
  if (reader != NULL) {
    try {
      std::string cDescription = reader->getDescription();
      description = c2jni(cDescription, env);
    } catch (...) {
      std::cerr << "Unable to get ZIM description" << std::endl;
    }
  }
  pthread_mutex_unlock(&readerLock);

  return description;
}

JNIEXPORT jboolean JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_getRandomPage(
    JNIEnv* env, jobject obj, jobject urlObj)
{
  jboolean retVal = JNI_FALSE;
  std::string cUrl;

  pthread_mutex_lock(&readerLock);
  try {
    if (reader != NULL) {
      std::string cUrl = reader->getRandomPageUrl();
      setStringObjValue(cUrl, urlObj, env);
      retVal = JNI_TRUE;
    }
  } catch (...) {
    std::cerr << "Unable to get random page" << std::endl;
  }
  pthread_mutex_unlock(&readerLock);

  return retVal;
}

JNIEXPORT void JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_setDataDirectory(
    JNIEnv* env, jobject obj, jstring dirStr)
{
  std::string cPath = jni2c(dirStr, env);

  pthread_mutex_lock(&readerLock);
  try {
    u_setDataDirectory(cPath.c_str());
  } catch (...) {
    std::cerr << "Unable to set data directory " << cPath << std::endl;
  }
  pthread_mutex_unlock(&readerLock);
}

JNIEXPORT jboolean JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_loadFulltextIndex(
    JNIEnv* env, jobject obj, jstring path)
{
  jboolean retVal = JNI_TRUE;
  std::string cPath = jni2c(path, env);

  pthread_mutex_lock(&searcherLock);
  try {
    if (searcher != NULL) {
      delete searcher;
    }
    if (!reader || !reader->hasFulltextIndex()) {
      // Use old API (no embedded full text index).
      searcher = new kiwix::Searcher(cPath, reader, "");
    } else {
      // Use the new API. We don't care about the human readable name as
      // we don't use it (in android).
      searcher = new kiwix::Searcher();
      searcher->add_reader(reader, "");
    }
  } catch (...) {
    searcher = NULL;
    retVal = JNI_FALSE;
    std::cerr << "Unable to load full text index " << cPath << std::endl;
  }
  pthread_mutex_unlock(&searcherLock);

  return retVal;
}

JNIEXPORT jstring JNICALL Java_org_kiwix_kiwixlib_JNIKiwix_indexedQuery(
    JNIEnv* env, jclass obj, jstring query, jint count)
{
  std::string cQuery = jni2c(query, env);
  unsigned int cCount = jni2c(count);
  kiwix::Result* p_result;
  std::string result;

  pthread_mutex_lock(&searcherLock);
  try {
    if (searcher != NULL) {
      searcher->search(cQuery, 0, count);
      while ((p_result = searcher->getNextResult())
             && !(p_result->get_title().empty())
             && !(p_result->get_url().empty())) {
        result += p_result->get_title() + "\n";
        delete p_result;
      }
    }
  } catch (...) {
    std::cerr << "Unable to make indexed query " << cQuery << std::endl;
  }
  pthread_mutex_unlock(&searcherLock);

  return env->NewStringUTF(result.c_str());
}

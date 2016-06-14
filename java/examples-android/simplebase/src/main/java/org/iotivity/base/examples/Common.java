/*
 * ******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import android.content.Context;
import android.widget.Toast;

import org.iotivity.base.OcPlatform;
import org.iotivity.base.ResourceProperty;

import java.text.DateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.EnumSet;
import java.util.Locale;

/**
 * This class charge of common part.
 */
public class Common {

    public static final int    DATA_SIZE          = 3000;
    public static final String COAP_TCP           = "coap+tcp://";
    public static String       TCP_ADDRESS        = "192.168.0.1";
    public static final String TCP_PORT           = ":8000";
    public static final String IP_ADDRESS         = "0.0.0.0";
    public static final int    IP_PORT            = 0;
    public static final String GET_COMMAND        = "get_command";
    public static final String STATE_KEY          = "state_key";
    public static final String STATE_GET          = "state_get";
    public static final String LARGE_KEY          = "large_key";
    public static final String LARGE_GET          = "large_get";
    public static final String RESOURCE_URI       = "/a/light";
    public static final String RESOURCE_TYPE      = "core.light";
    public static final String RESOURCE_INTERFACE = OcPlatform.DEFAULT_INTERFACE;
    public static final EnumSet<ResourceProperty> RESOURCE_PROPERTIES =
            EnumSet.of(ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE);

    public static String getDateCurrentTimeZone() {
        StringBuilder sb = new StringBuilder();
        try {
            Calendar calendar = Calendar.getInstance();
            calendar.setTimeInMillis(System.currentTimeMillis());
            DateFormat dateFormat = DateFormat.getDateTimeInstance(DateFormat.DEFAULT,
                                                                   DateFormat.DEFAULT,
                                                                   Locale.getDefault());
            Date currentTimeZone = calendar.getTime();
            sb.append(dateFormat.format(currentTimeZone));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return sb.toString();
    }

    public static void showToast(Context context, String msg) {
        Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
    }
}

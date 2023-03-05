// webrtc with gstreamer

#include <iostream>
#include <gst/gst.h>
#pragma comment(lib, "gstreamer-1.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "glib-2.0.lib")

static void on_negotiation_needed(GstElement* webrtc, gpointer user_data);
static void on_offer_created(GstPromise* promise, GstElement* webrtc);
static void on_incoming_stream(GstElement* webrtc, GstPad* pad, GstElement* pipe);

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";

    //GstElement* pipeline;
    //GstBus* bus;
    //GstMessage* msg;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    GstElement* pipe;

    pipe = gst_parse_launch("v4l2src ! queue ! vp8enc ! rtpvp8pay ! "
        "application/x-rtp,media=video,encoding-name=VP8,payload=96 !"
        " webrtcbin name=sendrecv", NULL);

    GstElement* webrtc;

    webrtc = gst_bin_get_by_name(GST_BIN(pipe), "sendrecv");
    g_assert(webrtc != NULL);

    /* This is the gstwebrtc entry point where we create the offer.
     * It will be called when the pipeline goes to PLAYING. */
    g_signal_connect(webrtc, "on-negotiation-needed",
        G_CALLBACK(on_negotiation_needed), NULL);
    /* We will transmit this ICE candidate to the remote using some
     * signalling. Incoming ICE candidates from the remote need to be
     * added by us too. */
    //g_signal_connect(webrtc, "on-ice-candidate",
    //    G_CALLBACK(send_ice_candidate_message), NULL);
    /* Incoming streams will be exposed via this signal */
    g_signal_connect(webrtc, "pad-added",
        G_CALLBACK(on_incoming_stream), pipe);
    /* Lifetime is the same as the pipeline itself */
    gst_object_unref(webrtc);



    gst_element_set_state(pipe, GST_STATE_NULL);
    gst_object_unref(pipe);

    ///* Build the pipeline */
    //pipeline =
    //    gst_parse_launch
    //    ("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
    //        nullptr);

    ///* Start playing */
    //gst_element_set_state(pipeline, GST_STATE_PLAYING);

    ///* Wait until error or EOS */
    //bus = gst_element_get_bus(pipeline);
    //msg =
    //    gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
    //        (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    ///* See next tutorial for proper error message handling/parsing */
    //if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
    //    g_error("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
    //        "variable set for more details.");
    //}

    ///* Free resources */
    //gst_message_unref(msg);
    //gst_object_unref(bus);
    //gst_element_set_state(pipeline, GST_STATE_NULL);
    //gst_object_unref(pipeline);

}


static void on_negotiation_needed(GstElement* webrtc, gpointer user_data)
{
    GstPromise* promise;

    promise = gst_promise_new_with_change_func(on_offer_created,
        user_data, NULL);
    g_signal_emit_by_name(webrtc, "create-offer", NULL,
        promise);
}

static void on_offer_created(GstPromise* promise, GstElement* webrtc)
{
    GstWebRTCSessionDescription* offer = NULL;
    const GstStructure* reply;
    gchar* desc;

    reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "offer",
        GST_TYPE_WEBRTC_SESSION_DESCRIPTION,
        &offer, NULL);
    gst_promise_unref(promise);

    /* We can edit this offer before setting and sending */
    g_signal_emit_by_name(webrtc,
        "set-local-description", offer, NULL);

    /* Implement this and send offer to peer using signalling */
    send_sdp_offer(offer);
    gst_webrtc_session_description_free(offer);
}

static void on_incoming_stream(GstElement* webrtc, GstPad* pad,
    GstElement* pipe)
{
    GstElement* play;

    play = gst_parse_bin_from_description(
        "queue ! vp8dec ! videoconvert ! autovideosink",
        TRUE, NULL);
    gst_bin_add(GST_BIN(pipe), play);

    /* Start displaying video */
    gst_element_sync_state_with_parent(play);
    gst_element_link(webrtc, play);
}
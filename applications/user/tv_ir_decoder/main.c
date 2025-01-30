#include <furi.h>
#include <gui/gui.h>
#include <notification/notification_messages.h>
#include <input/input.h>
#include <stream_buffer.h>
#include <toolbox/path.h>
#include <storage/storage.h>

typedef struct {
    FuriThread* thread;
    ViewPort* view_port;
    NotificationApp* notifications;
    Storage* storage;
    StreamBufferHandle_t stream_buffer;
    char current_tv[64];
    char current_model[64];
} TvIrDecoder;

static void tv_ir_decoder_draw_callback(Canvas* canvas, void* context) {
    TvIrDecoder* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 64, 10, AlignCenter, AlignTop, "TV IR Decoder");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas,
        64,
        25,
        AlignCenter,
        AlignTop,
        app->current_tv);
    canvas_draw_str_aligned(
        canvas,
        64,
        40,
        AlignCenter,
        AlignTop,
        app->current_model);
}

static void tv_ir_decoder_input_callback(InputEvent* input_event, void* context) {
    TvIrDecoder* app = context;
    if(input_event->type == InputTypePress) {
        switch(input_event->key) {
            case InputKeyBack:
                furi_thread_flags_set(furi_thread_get_id(app->thread), 1 << 0);
                break;
            default:
                break;
        }
    }
}

static int32_t tv_ir_decoder_worker(void* context) {
    TvIrDecoder* app = context;
    Storage* storage = app->storage;
    StreamBufferHandle_t stream_buffer = app->stream_buffer;

    // Implement the logic to iterate over .ir files and send them via IR
    // This is a placeholder for now
    while(true) {
        // Check for exit condition
        uint32_t flags = furi_thread_flags_wait(0x01, FuriFlagWaitAny, 100);
        if(flags & (1 << 0)) {
            break;
        }

        // Iterate through assets directory and send .ir files
        // Placeholder logic
        strcpy(app->current_tv, "Samsung");
        strcpy(app->current_model, "Model1");
        view_port_update(app->view_port);

        // Simulate sending IR signal and waiting for response
        furi_delay_ms(1000);
    }

    return 0;
}

int32_t tv_ir_decoder_app(void* p) {
    UNUSED(p);
    TvIrDecoder* app = malloc(sizeof(TvIrDecoder));
    app->storage = furi_record_open(RECORD_STORAGE);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->stream_buffer = xStreamBufferCreate(128, 1);

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, tv_ir_decoder_draw_callback, app);
    view_port_input_callback_set(app->view_port, tv_ir_decoder_input_callback, app);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, app->view_port, GuiLayerFullscreen);

    app->thread = furi_thread_alloc_ex("TvIrDecoderWorker", 2048, tv_ir_decoder_worker, app);
    furi_thread_start(app->thread);

    // Wait for the worker to finish
    furi_thread_join(app->thread);
    furi_thread_free(app->thread);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(gui, app->view_port);
    view_port_free(app->view_port);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_NOTIFICATION);
    xStreamBufferDelete(app->stream_buffer);
    free(app);

    return 0;
}

void* tv_ir_decoder_app_alloc() {
    return NULL;
}

void tv_ir_decoder_app_free(void* app) {
    UNUSED(app);
}

void tv_ir_decoder_app_start(void* app) {
    UNUSED(app);
    tv_ir_decoder_app(NULL);
}

void tv_ir_decoder_app_stop(void* app) {
    UNUSED(app);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <libgrss.h>

gchar *cnew_rss_feeds[] = {
    "http://rss.slashdot.org/Slashdot/slashdotMain",
    "https://feeds.twit.tv/floss.xml",
    "https://undeadly.org/cgi?action=rss",
    "http://www.NetBSD.org/changes/rss-netbsd.xml",
    "https://lobste.rs/t/programming.rss",
    "https://news.ycombinator.com/rss",
    NULL
};

gboolean read_complete = FALSE;
static void cnews_feed_fetched(GrssFeedsPool *, GrssFeedChannel *, GList *);

static void cnews_feed_fetched(GrssFeedsPool * pool, GrssFeedChannel * feed,
                               GList * items)
{
    GList *iter;
    GrssFeedItem *item;

    if (items == NULL) {
        printf("Error while fetching %s\n", grss_feed_channel_get_title(feed));
        return;
    }

    printf("Fetched from %s\n", grss_feed_channel_get_title(feed));

    for (iter = items; iter; iter = g_list_next(iter)) {
        item = (GrssFeedItem *) iter->data;
        printf("\t\t%s\n", grss_feed_item_get_title(item));
    }

    printf("\n \n");
}

int main(int argc, char *argv[])
{
    GList *iter = NULL;
    GList *list = NULL;
    GrssFeedChannel *feed = NULL;
    GrssFeedsPool *pool = NULL;
    GMainLoop *main_loop = NULL;
    register int i;

    main_loop = g_main_loop_new(NULL, FALSE);
    if (main_loop == NULL) {
        printf("Failed to created a new execution context : %s \n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (i = 0; cnew_rss_feeds[i] != NULL; i++) {
        feed = grss_feed_channel_new();
        grss_feed_channel_set_source(feed, cnew_rss_feeds[i]);
        grss_feed_channel_set_update_interval(feed, 1);
        list = g_list_prepend(list, feed);
    }

    list = g_list_reverse(list);

    pool = grss_feeds_pool_new();
    grss_feeds_pool_listen(pool, list);
    grss_feeds_pool_switch(pool, TRUE);

    g_signal_connect(pool, "feed-ready", G_CALLBACK(cnews_feed_fetched), NULL);
    g_main_loop_run(main_loop);

    for (iter = list; iter; iter = g_list_next(iter))
        g_object_unref(iter->data);

    g_object_unref(pool);
    return EXIT_SUCCESS;
}

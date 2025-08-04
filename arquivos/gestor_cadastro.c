#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

GtkBuilder *builder;
GtkWidget *window;
GtkStack *stack;
GtkListStore *modelo_armazenamento;

typedef struct usuario {
    int id;
    char nome[50];
    char email[50];
    struct usuario *proximo;
} user;

int id = 0;
user *cabecalho_user;
user *proximo_user;

void mensagem(const char *text, const char *secondary_text, const char *icon_name) {
    GtkWidget *mensagem_dialogo = GTK_WIDGET(gtk_builder_get_object(builder, "mensagem"));

    if (!mensagem_dialogo) {
        g_printerr("Falha ao encontrar o widget 'mensagem'.\n");
        return;
    }

    g_object_set(mensagem_dialogo, "text", text, NULL);
    g_object_set(mensagem_dialogo, "secondary_text", secondary_text, NULL);
    g_object_set(mensagem_dialogo, "icon_name", icon_name, NULL);

    gtk_widget_show_all(mensagem_dialogo);
    gtk_dialog_run(GTK_DIALOG(mensagem_dialogo));
    gtk_widget_hide(mensagem_dialogo);
}

void login(const char *usuario, const char *senha, gboolean lembrar) {
    if ((strcmp(usuario, "admin") == 0) && (strcmp(senha, "admin") == 0)) {
        mensagem("Bem Vindo!", "Usuario logado com sucesso!", "emblem-default");
        gtk_stack_set_visible_child_name(stack, "view_inicial");
        g_object_set(window, "icon_name", "avatar-default", NULL);
    } else {
        mensagem("Erro!", "Usuario ou senha incorretos!", "dialog-error");
    }
}

void on_button_login_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *entry_usuario = GTK_ENTRY(gtk_builder_get_object(builder, "usuario"));
    GtkEntry *entry_senha = GTK_ENTRY(gtk_builder_get_object(builder, "senha"));
    GtkToggleButton *chk_lembrar = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "lembrar"));

    if (!entry_usuario || !entry_senha || !chk_lembrar) {
        g_printerr("Falha ao encontrar os widgets da interface.\n");
        return;
    }

    const char *email = gtk_entry_get_text(entry_usuario);
    const char *senha = gtk_entry_get_text(entry_senha);
    gboolean lembrar = gtk_toggle_button_get_active(chk_lembrar);

    login(email, senha, lembrar);
}

void on_main_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void on_button_cadastrar_inicial_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_cadastro");
}

void on_play_game_clicked(GtkWidget *widget, gpointer data) {
#if defined(_WIN32)
    system("start https://trex-runner.com/night/");
#elif defined(__APPLE__)
    system("open https://trex-runner.com/night/");
#else
    system("xdg-open https://trex-runner.com/night/");
#endif
}

void on_button_listar_inicial_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_listar");
}

void on_button_sair_inicial_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_login");
    g_object_set(window, "icon_name", "changes-prevent-symbolic", NULL);
}

void on_button_cad_voltar_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_inicial");
}

void on_button_cadastrar_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *entry_nome = GTK_ENTRY(gtk_builder_get_object(builder, "cad_nome"));
    GtkEntry *entry_email = GTK_ENTRY(gtk_builder_get_object(builder, "cad_email"));

    const char *cad_nome = gtk_entry_get_text(entry_nome);
    const char *cad_email = gtk_entry_get_text(entry_email);

    if (strcmp(cad_nome, "") == 0 || strcmp(cad_email, "") == 0) {
        mensagem("Erro", "Campos 'Nome' ou 'Email' obrigatórios", "dialog-error");
    } else {
        id++;
        proximo_user->id = id;
        strcpy(proximo_user->nome, cad_nome);
        strcpy(proximo_user->email, cad_email);

        g_print("id: %d | nome: %s | email: %s\n",
                proximo_user->id,
                proximo_user->nome,
                proximo_user->email);

        char texto[100];
        g_snprintf(texto, 100, "Usuario %s cadastrado!", proximo_user->nome);
        mensagem("Aviso", texto, "dialog-emblem-default");

        proximo_user->proximo = (user *)malloc(sizeof(user));
        proximo_user = proximo_user->proximo;
        proximo_user->proximo = NULL;
    }
}

void on_button_listar_clicked(GtkWidget *widget, gpointer data) {
    GtkTreeIter iter;
    gtk_list_store_clear(modelo_armazenamento);

    user *atual = cabecalho_user;

    while (atual != NULL && atual->proximo != NULL) {
        g_print("id: %d | nome: %s | email: %s\n",
                atual->id,
                atual->nome,
                atual->email);

        gtk_list_store_append(modelo_armazenamento, &iter);
        gtk_list_store_set(modelo_armazenamento, &iter,
                           0, atual->id,
                           1, atual->nome,
                           2, atual->email,
                           -1);

        atual = atual->proximo;
    }
}

void on_button_listar_voltar_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_inicial");
}

int main(int argc, char *argv[]) {
    cabecalho_user = (user *)malloc(sizeof(user));
    proximo_user = cabecalho_user;
    proximo_user->proximo = NULL;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("ui.glade");
    if (!builder) {
        g_printerr("Falha ao carregar o arquivo ui.glade\n");
        return -1;
    }

    gtk_builder_add_callback_symbols(
        builder,
        "on_button_login_clicked", G_CALLBACK(on_button_login_clicked),
        "on_main_window_destroy", G_CALLBACK(on_main_window_destroy),
        "on_button_cadastrar_inicial_clicked", G_CALLBACK(on_button_cadastrar_inicial_clicked),
        "on_button_listar_inicial_clicked", G_CALLBACK(on_button_listar_inicial_clicked),
        "on_button_sair_inicial_clicked", G_CALLBACK(on_button_sair_inicial_clicked),
        "on_button_cad_voltar_clicked", G_CALLBACK(on_button_cad_voltar_clicked),
        "on_button_cadastrar_clicked", G_CALLBACK(on_button_cadastrar_clicked),
        "on_button_listar_clicked", G_CALLBACK(on_button_listar_clicked),
        "on_button_listar_voltar_clicked", G_CALLBACK(on_button_listar_voltar_clicked),
        "on_play_game_clicked", G_CALLBACK(on_play_game_clicked),
        NULL
    );

    gtk_builder_connect_signals(builder, NULL);

    stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    modelo_armazenamento = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));

    if (!window || !stack) {
        g_printerr("Falha ao encontrar a janela ou o stack na interface.\n");
        return -1;
    }

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}


#include "Entity.h"

Entity::Entity(int val1, int val2, int height, int width ){
    x = val1;
    y = val2;
    h = height;
    w = width;

    Entity_Rect.x = val1;
    Entity_Rect.y = val2;
    Entity_Rect.h = height;
    Entity_Rect.w = width;

    CreateEntity_Texture();
} 

int Entity::Get_x(){
    return x;
}
// Travis asdf
int Entity::Get_y(){
    return y;
}

int Entity::Get_height(){
    return h;
}

int Entity::Get_width(){
    return w;
}

void Entity::Set_x(int val){
    x = val;
    Entity_Rect.x = val;
}

void Entity::Set_y(int val){
    y = val;
    Entity_Rect.y = val;
}

SDL_Rect Entity::GetEntity_rect(){
    return Entity_Rect;
}

void Entity::SetEntity_texture(string Path){
    Entity_Image.Cargar_Textura(Path);
}

void Entity::CreateEntity_Texture(){
    Entity_Image.Create_Textura(w,h);
}

void Entity::Render_Entity(){
    Entity_Image.Render(x,y);
}


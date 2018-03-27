startup<-read.table("/home/wyatt/Documents/Arboreal/Source/Data/startup_time.txt", header = TRUE)

startup$tags <- factor(startup$tags) 

plot(time~files, data = startup)
plot(time~tags, data = startup)

fit<-lm(log(time)~ files + log(tags)+ I(files^2) +I(files^3), data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Tags", main="Tags Residual Plot")
